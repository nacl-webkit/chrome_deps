// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tile_manager.h"

#include <algorithm>

#include "base/bind.h"
#include "base/debug/trace_event.h"
#include "base/json/json_writer.h"
#include "base/logging.h"
#include "cc/platform_color.h"
#include "cc/raster_worker_pool.h"
#include "cc/resource_pool.h"
#include "cc/tile.h"
#include "third_party/skia/include/core/SkDevice.h"

namespace cc {

namespace {

// If we raster too fast we become upload bound, and pending
// uploads consume memory. For maximum upload throughput, we would
// want to allow for upload_throughput * pipeline_time of pending
// uploads, after which we are just wasting memory. Since we don't
// know our upload throughput yet, this just caps our memory usage.
#if defined(OS_ANDROID)
// For reference, the Nexus10 can upload 1MB in about 2.5ms.
// Assuming a three frame deep pipeline this implies ~20MB.
const int kMaxPendingUploadBytes = 20 * 1024 * 1024;
#else
const int kMaxPendingUploadBytes = 100 * 1024 * 1024;
#endif

// Determine bin based on three categories of tiles: things we need now,
// things we need soon, and eventually.
inline TileManagerBin BinFromTilePriority(const TilePriority& prio) {
  if (!prio.is_live)
    return NEVER_BIN;

  // The amount of time for which we want to have prepainting coverage.
  const double prepainting_window_time_seconds = 1.0;
  const double backfling_guard_distance_pixels = 314.0;

  // Explicitly limit how far ahead we will prepaint to limit memory usage.
  if (prio.distance_to_visible_in_pixels >
      TilePriority::kMaxDistanceInContentSpace)
    return NEVER_BIN;

  if (prio.time_to_visible_in_seconds == std::numeric_limits<float>::max())
    return NEVER_BIN;

  if (prio.time_to_visible_in_seconds == 0 ||
      prio.distance_to_visible_in_pixels < backfling_guard_distance_pixels)
    return NOW_BIN;

  if (prio.resolution == NON_IDEAL_RESOLUTION)
    return EVENTUALLY_BIN;

  if (prio.time_to_visible_in_seconds < prepainting_window_time_seconds)
    return SOON_BIN;

  return EVENTUALLY_BIN;
}

std::string ValueToString(scoped_ptr<base::Value> value)
{
  std::string str;
  base::JSONWriter::Write(value.get(), &str);
  return str;
}

}  // namespace

scoped_ptr<base::Value> TileManagerBinAsValue(TileManagerBin bin) {
  switch (bin) {
  case NOW_BIN:
      return scoped_ptr<base::Value>(base::Value::CreateStringValue(
          "NOW_BIN"));
  case SOON_BIN:
      return scoped_ptr<base::Value>(base::Value::CreateStringValue(
          "SOON_BIN"));
  case EVENTUALLY_BIN:
      return scoped_ptr<base::Value>(base::Value::CreateStringValue(
          "EVENTUALLY_BIN"));
  case NEVER_BIN:
      return scoped_ptr<base::Value>(base::Value::CreateStringValue(
          "NEVER_BIN"));
  default:
      DCHECK(false) << "Unrecognized TileManagerBin value";
      return scoped_ptr<base::Value>(base::Value::CreateStringValue(
          "<unknown TileManagerBin value>"));
  }
}

ManagedTileState::ManagedTileState()
    : can_use_gpu_memory(false),
      can_be_freed(true),
      resource_is_being_initialized(false),
      contents_swizzled(false),
      need_to_gather_pixel_refs(true),
      gpu_memmgr_stats_bin(NEVER_BIN),
      raster_state(IDLE_STATE) {
  for (int i = 0; i < NUM_TREES; ++i)
    tree_bin[i] = NEVER_BIN;
}

ManagedTileState::~ManagedTileState() {
  DCHECK(!resource);
  DCHECK(!resource_is_being_initialized);
}

TileManager::TileManager(
    TileManagerClient* client,
    ResourceProvider* resource_provider,
    size_t num_raster_threads,
    bool record_rendering_stats)
    : client_(client),
      resource_pool_(ResourcePool::Create(resource_provider)),
      raster_worker_pool_(RasterWorkerPool::Create(num_raster_threads, record_rendering_stats)),
      manage_tiles_pending_(false),
      manage_tiles_call_count_(0),
      bytes_pending_set_pixels_(0),
      ever_exceeded_memory_budget_(false),
      record_rendering_stats_(record_rendering_stats) {
  for (int i = 0; i < NUM_STATES; ++i) {
    for (int j = 0; j < NUM_TREES; ++j) {
      for (int k = 0; k < NUM_BINS; ++k)
        raster_state_count_[i][j][k] = 0;
    }
  }
}

TileManager::~TileManager() {
  // Reset global state and manage. This should cause
  // our memory usage to drop to zero.
  global_state_ = GlobalStateThatImpactsTilePriority();
  AssignGpuMemoryToTiles();
  // This should finish all pending tasks and release any uninitialized
  // resources.
  raster_worker_pool_.reset();
  CheckForCompletedTileUploads();
  DCHECK(tiles_with_pending_set_pixels_.size() == 0);
  DCHECK(tiles_.size() == 0);
}

void TileManager::SetGlobalState(
    const GlobalStateThatImpactsTilePriority& global_state) {
  global_state_ = global_state;
  resource_pool_->SetMaxMemoryUsageBytes(global_state_.memory_limit_in_bytes);
  ScheduleManageTiles();
}

void TileManager::RegisterTile(Tile* tile) {
  tiles_.push_back(tile);

  const ManagedTileState& mts = tile->managed_state();
  for (int i = 0; i < NUM_TREES; ++i)
    ++raster_state_count_[mts.raster_state][i][mts.tree_bin[i]];

  ScheduleManageTiles();
}

void TileManager::UnregisterTile(Tile* tile) {
  for (TileList::iterator it = tiles_with_image_decoding_tasks_.begin();
       it != tiles_with_image_decoding_tasks_.end(); it++) {
    if (*it == tile) {
      tiles_with_image_decoding_tasks_.erase(it);
      break;
    }
  }
  for (TileVector::iterator it = tiles_that_need_to_be_rasterized_.begin();
       it != tiles_that_need_to_be_rasterized_.end(); it++) {
    if (*it == tile) {
      tiles_that_need_to_be_rasterized_.erase(it);
      break;
    }
  }
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); it++) {
    if (*it == tile) {
      const ManagedTileState& mts = tile->managed_state();
      for (int i = 0; i < NUM_TREES; ++i)
        --raster_state_count_[mts.raster_state][i][mts.tree_bin[i]];
      FreeResourcesForTile(tile);
      tiles_.erase(it);
      return;
    }
  }
  DCHECK(false) << "Could not find tile version.";
}

class BinComparator {
public:
  bool operator() (const Tile* a, const Tile* b) const {
    const ManagedTileState& ams = a->managed_state();
    const ManagedTileState& bms = b->managed_state();
    if (ams.bin[HIGH_PRIORITY_BIN] != bms.bin[HIGH_PRIORITY_BIN])
      return ams.bin[HIGH_PRIORITY_BIN] < bms.bin[HIGH_PRIORITY_BIN];

    if (ams.bin[LOW_PRIORITY_BIN] != bms.bin[LOW_PRIORITY_BIN])
      return ams.bin[LOW_PRIORITY_BIN] < bms.bin[LOW_PRIORITY_BIN];

    if (ams.resolution != bms.resolution)
      return ams.resolution < bms.resolution;

    if (ams.time_to_needed_in_seconds !=  bms.time_to_needed_in_seconds)
      return ams.time_to_needed_in_seconds < bms.time_to_needed_in_seconds;

    gfx::Rect a_rect = a->content_rect();
    gfx::Rect b_rect = b->content_rect();
    if (a_rect.y() != b_rect.y())
      return a_rect.y() < b_rect.y();
    return a_rect.x() < b_rect.x();
  }
};

void TileManager::SortTiles() {
  TRACE_EVENT0("cc", "TileManager::SortTiles");

  // Sort by bin, resolution and time until needed.
  std::sort(tiles_.begin(), tiles_.end(), BinComparator());
}

void TileManager::ManageTiles() {
  TRACE_EVENT0("cc", "TileManager::ManageTiles");
  manage_tiles_pending_ = false;
  ++manage_tiles_call_count_;

  const TreePriority tree_priority = global_state_.tree_priority;
  TRACE_COUNTER_ID1("cc", "TileCount", this, tiles_.size());

  // For each tree, bin into different categories of tiles.
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
    Tile* tile = *it;
    ManagedTileState& mts = tile->managed_state();

    TilePriority prio[NUM_BIN_PRIORITIES];
    switch (tree_priority) {
      case SAME_PRIORITY_FOR_BOTH_TREES:
        prio[HIGH_PRIORITY_BIN] = prio[LOW_PRIORITY_BIN] =
            tile->combined_priority();
        break;
      case SMOOTHNESS_TAKES_PRIORITY:
        prio[HIGH_PRIORITY_BIN] = tile->priority(ACTIVE_TREE);
        prio[LOW_PRIORITY_BIN] = tile->priority(PENDING_TREE);
        break;
      case NEW_CONTENT_TAKES_PRIORITY:
        prio[HIGH_PRIORITY_BIN] = tile->priority(PENDING_TREE);
        prio[LOW_PRIORITY_BIN] = tile->priority(ACTIVE_TREE);
        break;
    }

    mts.resolution = prio[HIGH_PRIORITY_BIN].resolution;
    mts.time_to_needed_in_seconds =
        prio[HIGH_PRIORITY_BIN].time_to_visible_in_seconds;
    mts.bin[HIGH_PRIORITY_BIN] = BinFromTilePriority(prio[HIGH_PRIORITY_BIN]);
    mts.bin[LOW_PRIORITY_BIN] = BinFromTilePriority(prio[LOW_PRIORITY_BIN]);
    mts.gpu_memmgr_stats_bin = BinFromTilePriority(tile->combined_priority());

    DidTileBinChange(tile,
                     BinFromTilePriority(tile->priority(ACTIVE_TREE)),
                     ACTIVE_TREE);
    DidTileBinChange(tile,
                     BinFromTilePriority(tile->priority(PENDING_TREE)),
                     PENDING_TREE);
  }

  // Memory limit policy works by mapping some bin states to the NEVER bin.
  TileManagerBin bin_map[NUM_BINS];
  if (global_state_.memory_limit_policy == ALLOW_NOTHING) {
    bin_map[NOW_BIN] = NEVER_BIN;
    bin_map[SOON_BIN] = NEVER_BIN;
    bin_map[EVENTUALLY_BIN] = NEVER_BIN;
    bin_map[NEVER_BIN] = NEVER_BIN;
  } else if (global_state_.memory_limit_policy == ALLOW_ABSOLUTE_MINIMUM) {
    bin_map[NOW_BIN] = NOW_BIN;
    bin_map[SOON_BIN] = NEVER_BIN;
    bin_map[EVENTUALLY_BIN] = NEVER_BIN;
    bin_map[NEVER_BIN] = NEVER_BIN;
  } else if (global_state_.memory_limit_policy == ALLOW_PREPAINT_ONLY) {
    bin_map[NOW_BIN] = NOW_BIN;
    bin_map[SOON_BIN] = SOON_BIN;
    bin_map[EVENTUALLY_BIN] = NEVER_BIN;
    bin_map[NEVER_BIN] = NEVER_BIN;
  } else {
    bin_map[NOW_BIN] = NOW_BIN;
    bin_map[SOON_BIN] = SOON_BIN;
    bin_map[EVENTUALLY_BIN] = EVENTUALLY_BIN;
    bin_map[NEVER_BIN] = NEVER_BIN;
  }
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
    Tile* tile = *it;
    ManagedTileState& mts = tile->managed_state();
    for (int i = 0; i < NUM_BIN_PRIORITIES; ++i)
      mts.bin[i] = bin_map[mts.bin[i]];

    DidTileBinChange(tile, bin_map[mts.tree_bin[ACTIVE_TREE]], ACTIVE_TREE);
    DidTileBinChange(tile, bin_map[mts.tree_bin[PENDING_TREE]], PENDING_TREE);
  }

  SortTiles();

  // Assign gpu memory and determine what tiles need to be rasterized.
  AssignGpuMemoryToTiles();

  TRACE_EVENT_INSTANT1("cc", "DidManage", "state", ValueToString(AsValue()));

  // Finally, kick the rasterizer.
  DispatchMoreTasks();
}

void TileManager::CheckForCompletedTileUploads() {
  while (!tiles_with_pending_set_pixels_.empty()) {
    Tile* tile = tiles_with_pending_set_pixels_.front();
    DCHECK(tile->managed_state().resource);

    // Set pixel tasks complete in the order they are posted.
    if (!resource_pool_->resource_provider()->didSetPixelsComplete(
          tile->managed_state().resource->id())) {
      break;
    }

    if (tile->priority(ACTIVE_TREE).distance_to_visible_in_pixels == 0 &&
        tile->priority(ACTIVE_TREE).resolution == HIGH_RESOLUTION)
      client_->DidUploadVisibleHighResolutionTile();

    // It's now safe to release the pixel buffer.
    resource_pool_->resource_provider()->releasePixelBuffer(
        tile->managed_state().resource->id());

    DidFinishTileInitialization(tile);

    bytes_pending_set_pixels_ -= tile->bytes_consumed_if_allocated();
    DidTileRasterStateChange(tile, IDLE_STATE);
    tiles_with_pending_set_pixels_.pop();
  }

  DispatchMoreTasks();
}

void TileManager::GetMemoryStats(
    size_t* memoryRequiredBytes,
    size_t* memoryNiceToHaveBytes,
    size_t* memoryUsedBytes) const {
  *memoryRequiredBytes = 0;
  *memoryNiceToHaveBytes = 0;
  *memoryUsedBytes = 0;
  for(size_t i = 0; i < tiles_.size(); i++) {
    const Tile* tile = tiles_[i];
    const ManagedTileState& mts = tile->managed_state();
    size_t tile_bytes = tile->bytes_consumed_if_allocated();
    if (mts.gpu_memmgr_stats_bin == NOW_BIN)
      *memoryRequiredBytes += tile_bytes;
    if (mts.gpu_memmgr_stats_bin != NEVER_BIN)
      *memoryNiceToHaveBytes += tile_bytes;
    if (mts.can_use_gpu_memory)
      *memoryUsedBytes += tile_bytes;
  }
}

scoped_ptr<base::Value> TileManager::AsValue() const {
    scoped_ptr<base::DictionaryValue> state(new base::DictionaryValue());
    state->SetInteger("tile_count", tiles_.size());

    state->Set("global_state", global_state_.AsValue().release());

    state->Set("memory_requirements", GetMemoryRequirementsAsValue().release());
    return state.PassAs<base::Value>();
}

scoped_ptr<base::Value> TileManager::GetMemoryRequirementsAsValue() const {
  scoped_ptr<base::DictionaryValue> requirements(
      new base::DictionaryValue());

  size_t memoryRequiredBytes;
  size_t memoryNiceToHaveBytes;
  size_t memoryUsedBytes;
  GetMemoryStats(&memoryRequiredBytes,
                 &memoryNiceToHaveBytes,
                 &memoryUsedBytes);
  requirements->SetInteger("memory_required_bytes", memoryRequiredBytes);
  requirements->SetInteger("memory_nice_to_have_bytes", memoryNiceToHaveBytes);
  requirements->SetInteger("memory_used_bytes", memoryUsedBytes);
  return requirements.PassAs<base::Value>();
}

void TileManager::GetRenderingStats(RenderingStats* stats) {
  CHECK(record_rendering_stats_);
  raster_worker_pool_->GetRenderingStats(stats);
  stats->totalDeferredImageCacheHitCount =
      rendering_stats_.totalDeferredImageCacheHitCount;
  stats->totalImageGatheringCount = rendering_stats_.totalImageGatheringCount;
  stats->totalImageGatheringTime =
      rendering_stats_.totalImageGatheringTime;
}

bool TileManager::HasPendingWorkScheduled(WhichTree tree) const {
  // Always true when ManageTiles() call is pending.
  if (manage_tiles_pending_)
    return true;

  for (int i = 0; i < NUM_STATES; ++i) {
    switch (i) {
      case WAITING_FOR_RASTER_STATE:
      case RASTER_STATE:
      case SET_PIXELS_STATE:
        for (int j = 0; j < NEVER_BIN; ++j) {
          if (raster_state_count_[i][tree][j])
            return true;
        }
        break;
      case IDLE_STATE:
        break;
      default:
        NOTREACHED();
    }
  }

  return false;
}

void TileManager::AssignGpuMemoryToTiles() {
  TRACE_EVENT0("cc", "TileManager::AssignGpuMemoryToTiles");
  // Some memory cannot be released. Figure out which.
  size_t unreleasable_bytes = 0;
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
    Tile* tile = *it;
    if (!tile->managed_state().can_be_freed)
      unreleasable_bytes += tile->bytes_consumed_if_allocated();
  }

  // Now give memory out to the tiles until we're out, and build
  // the needs-to-be-rasterized queue.
  tiles_that_need_to_be_rasterized_.erase(
      tiles_that_need_to_be_rasterized_.begin(),
      tiles_that_need_to_be_rasterized_.end());

  // Reset the image decoding list so that we don't mess up with tile
  // priorities. Tiles will be added to the image decoding list again
  // when DispatchMoreTasks() is called.
  tiles_with_image_decoding_tasks_.clear();

  // By clearing the tiles_that_need_to_be_rasterized_ vector and
  // tiles_with_image_decoding_tasks_ list above we move all tiles
  // currently waiting for raster to idle state.
  // Call DidTileRasterStateChange() for each of these tiles to
  // have this state change take effect.
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
    Tile* tile = *it;
    if (tile->managed_state().raster_state == WAITING_FOR_RASTER_STATE)
      DidTileRasterStateChange(tile, IDLE_STATE);
  }

  size_t bytes_allocatable = global_state_.memory_limit_in_bytes - unreleasable_bytes;
  size_t bytes_that_exceeded_memory_budget = 0;
  size_t bytes_left = bytes_allocatable;
  for (TileVector::iterator it = tiles_.begin(); it != tiles_.end(); ++it) {
    Tile* tile = *it;
    size_t tile_bytes = tile->bytes_consumed_if_allocated();
    ManagedTileState& managed_tile_state = tile->managed_state();
    if (!managed_tile_state.can_be_freed)
      continue;
    if (managed_tile_state.bin[HIGH_PRIORITY_BIN] == NEVER_BIN &&
        managed_tile_state.bin[LOW_PRIORITY_BIN] == NEVER_BIN) {
      managed_tile_state.can_use_gpu_memory = false;
      FreeResourcesForTile(tile);
      continue;
    }
    if (tile_bytes > bytes_left) {
      managed_tile_state.can_use_gpu_memory = false;
      bytes_that_exceeded_memory_budget += tile_bytes;
      FreeResourcesForTile(tile);
      continue;
    }
    bytes_left -= tile_bytes;
    managed_tile_state.can_use_gpu_memory = true;
    if (!managed_tile_state.resource &&
        !managed_tile_state.resource_is_being_initialized) {
      tiles_that_need_to_be_rasterized_.push_back(tile);
      DidTileRasterStateChange(tile, WAITING_FOR_RASTER_STATE);
    }
  }

  ever_exceeded_memory_budget_ |= bytes_that_exceeded_memory_budget > 0;
  if (ever_exceeded_memory_budget_) {
      TRACE_COUNTER_ID2("cc", "over_memory_budget", this,
                        "budget", global_state_.memory_limit_in_bytes,
                        "over", bytes_that_exceeded_memory_budget);
  }
  memory_stats_from_last_assign_.bytes_allocated =
      bytes_allocatable - bytes_left;
  memory_stats_from_last_assign_.bytes_unreleasable = unreleasable_bytes;
  memory_stats_from_last_assign_.bytes_over =
      bytes_that_exceeded_memory_budget;

  // Reverse two tiles_that_need_* vectors such that pop_back gets
  // the highest priority tile.
  std::reverse(
      tiles_that_need_to_be_rasterized_.begin(),
      tiles_that_need_to_be_rasterized_.end());
}

void TileManager::FreeResourcesForTile(Tile* tile) {
  ManagedTileState& managed_tile_state = tile->managed_state();
  DCHECK(managed_tile_state.can_be_freed);
  if (managed_tile_state.resource)
    resource_pool_->ReleaseResource(managed_tile_state.resource.Pass());
}

bool TileManager::CanDispatchRasterTask(Tile* tile) {
  if (raster_worker_pool_->IsBusy())
    return false;
  size_t new_bytes_pending = bytes_pending_set_pixels_;
  new_bytes_pending += tile->bytes_consumed_if_allocated();
  return new_bytes_pending <= kMaxPendingUploadBytes;
}

void TileManager::DispatchMoreTasks() {
  // Because tiles in the image decoding list have higher priorities, we
  // need to process those tiles first before we start to handle the tiles
  // in the need_to_be_rasterized queue.
  for(TileList::iterator it = tiles_with_image_decoding_tasks_.begin();
      it != tiles_with_image_decoding_tasks_.end(); ) {
    DispatchImageDecodeTasksForTile(*it);
    ManagedTileState& managed_state = (*it)->managed_state();
    if (managed_state.pending_pixel_refs.empty()) {
      if (!CanDispatchRasterTask(*it))
        return;
      DispatchOneRasterTask(*it);
      tiles_with_image_decoding_tasks_.erase(it++);
    } else {
      ++it;
    }
  }

  // Process all tiles in the need_to_be_rasterized queue. If a tile has
  // image decoding tasks, put it to the back of the image decoding list.
  while (!tiles_that_need_to_be_rasterized_.empty()) {
    Tile* tile = tiles_that_need_to_be_rasterized_.back();
    DispatchImageDecodeTasksForTile(tile);
    ManagedTileState& managed_state = tile->managed_state();
    if (!managed_state.pending_pixel_refs.empty()) {
      tiles_with_image_decoding_tasks_.push_back(tile);
    } else {
      if (!CanDispatchRasterTask(tile))
        return;
      DispatchOneRasterTask(tile);
    }
    tiles_that_need_to_be_rasterized_.pop_back();
  }
}

void TileManager::GatherPixelRefsForTile(Tile* tile) {
  TRACE_EVENT0("cc", "TileManager::GatherPixelRefsForTile");
  ManagedTileState& managed_state = tile->managed_state();
  if (managed_state.need_to_gather_pixel_refs) {
    base::TimeTicks gather_begin_time;
    if (record_rendering_stats_)
      gather_begin_time = base::TimeTicks::Now();
    tile->picture_pile()->GatherPixelRefs(
        tile->content_rect_,
        tile->contents_scale_,
        managed_state.pending_pixel_refs);
    managed_state.need_to_gather_pixel_refs = false;
    if (record_rendering_stats_) {
      rendering_stats_.totalImageGatheringCount++;
      rendering_stats_.totalImageGatheringTime +=
          base::TimeTicks::Now() - gather_begin_time;
    }
  }
}

void TileManager::DispatchImageDecodeTasksForTile(Tile* tile) {
  GatherPixelRefsForTile(tile);
  std::list<skia::LazyPixelRef*>& pending_pixel_refs =
      tile->managed_state().pending_pixel_refs;
  std::list<skia::LazyPixelRef*>::iterator it = pending_pixel_refs.begin();
  while (it != pending_pixel_refs.end()) {
    if (pending_decode_tasks_.end() != pending_decode_tasks_.find(
        (*it)->getGenerationID())) {
      ++it;
      continue;
    }
    // TODO(qinmin): passing correct image size to PrepareToDecode().
    if ((*it)->PrepareToDecode(skia::LazyPixelRef::PrepareParams())) {
      rendering_stats_.totalDeferredImageCacheHitCount++;
      pending_pixel_refs.erase(it++);
    } else {
      if (raster_worker_pool_->IsBusy())
        return;
      DispatchOneImageDecodeTask(tile, *it);
      ++it;
    }
  }
}

void TileManager::DispatchOneImageDecodeTask(
    scoped_refptr<Tile> tile, skia::LazyPixelRef* pixel_ref) {
  TRACE_EVENT0("cc", "TileManager::DispatchOneImageDecodeTask");
  uint32_t pixel_ref_id = pixel_ref->getGenerationID();
  DCHECK(pending_decode_tasks_.end() ==
      pending_decode_tasks_.find(pixel_ref_id));
  pending_decode_tasks_[pixel_ref_id] = pixel_ref;

  raster_worker_pool_->PostTaskAndReply(
      base::Bind(&TileManager::RunImageDecodeTask, pixel_ref),
      base::Bind(&TileManager::OnImageDecodeTaskCompleted,
                 base::Unretained(this),
                 tile,
                 pixel_ref_id));
}

void TileManager::OnImageDecodeTaskCompleted(
    scoped_refptr<Tile> tile, uint32_t pixel_ref_id) {
  TRACE_EVENT0("cc", "TileManager::OnImageDecodeTaskCompleted");
  pending_decode_tasks_.erase(pixel_ref_id);

  for (TileList::iterator it = tiles_with_image_decoding_tasks_.begin();
      it != tiles_with_image_decoding_tasks_.end(); ++it) {
    std::list<skia::LazyPixelRef*>& pixel_refs =
        (*it)->managed_state().pending_pixel_refs;
    for (std::list<skia::LazyPixelRef*>::iterator pixel_it =
        pixel_refs.begin(); pixel_it != pixel_refs.end(); ++pixel_it) {
      if (pixel_ref_id == (*pixel_it)->getGenerationID()) {
        pixel_refs.erase(pixel_it);
        break;
      }
    }
  }

  DispatchMoreTasks();
}

void TileManager::DispatchOneRasterTask(scoped_refptr<Tile> tile) {
  TRACE_EVENT0("cc", "TileManager::DispatchOneRasterTask");
  ManagedTileState& managed_tile_state = tile->managed_state();
  DCHECK(managed_tile_state.can_use_gpu_memory);
  scoped_ptr<ResourcePool::Resource> resource =
      resource_pool_->AcquireResource(tile->tile_size_.size(), tile->format_);
  resource_pool_->resource_provider()->acquirePixelBuffer(resource->id());

  managed_tile_state.resource_is_being_initialized = true;
  managed_tile_state.can_be_freed = false;

  DidTileRasterStateChange(tile, RASTER_STATE);

  ResourceProvider::ResourceId resource_id = resource->id();

  raster_worker_pool_->PostRasterTaskAndReply(
      tile->picture_pile(),
      base::Bind(&TileManager::RunRasterTask,
                 resource_pool_->resource_provider()->mapPixelBuffer(
                     resource_id),
                 tile->content_rect_,
                 tile->contents_scale()),
      base::Bind(&TileManager::OnRasterTaskCompleted,
                 base::Unretained(this),
                 tile,
                 base::Passed(&resource),
                 manage_tiles_call_count_));
}

void TileManager::OnRasterTaskCompleted(
    scoped_refptr<Tile> tile,
    scoped_ptr<ResourcePool::Resource> resource,
    int manage_tiles_call_count_when_dispatched) {
  TRACE_EVENT0("cc", "TileManager::OnRasterTaskCompleted");

  // Release raster resources.
  resource_pool_->resource_provider()->unmapPixelBuffer(resource->id());

  ManagedTileState& managed_tile_state = tile->managed_state();
  managed_tile_state.can_be_freed = true;

  // Tile can be freed after the completion of the raster task. Call
  // AssignGpuMemoryToTiles() to re-assign gpu memory to highest priority
  // tiles if ManageTiles() was called since task was dispatched. The result
  // of this could be that this tile is no longer allowed to use gpu
  // memory and in that case we need to abort initialization and free all
  // associated resources before calling DispatchMoreTasks().
  if (manage_tiles_call_count_when_dispatched != manage_tiles_call_count_)
    AssignGpuMemoryToTiles();

  // Finish resource initialization if |can_use_gpu_memory| is true.
  if (managed_tile_state.can_use_gpu_memory) {
    // The component order may be bgra if we're uploading bgra pixels to rgba
    // texture. Mark contents as swizzled if image component order is
    // different than texture format.
    managed_tile_state.contents_swizzled =
        !PlatformColor::sameComponentOrder(tile->format_);

    // Tile resources can't be freed until upload has completed.
    managed_tile_state.can_be_freed = false;

    resource_pool_->resource_provider()->beginSetPixels(resource->id());
    resource_pool_->resource_provider()->shallowFlushIfSupported();
    managed_tile_state.resource = resource.Pass();

    bytes_pending_set_pixels_ += tile->bytes_consumed_if_allocated();
    DidTileRasterStateChange(tile, SET_PIXELS_STATE);
    tiles_with_pending_set_pixels_.push(tile);
  } else {
    resource_pool_->resource_provider()->releasePixelBuffer(resource->id());
    resource_pool_->ReleaseResource(resource.Pass());
    managed_tile_state.resource_is_being_initialized = false;
    DidTileRasterStateChange(tile, IDLE_STATE);
  }

  DispatchMoreTasks();
}

void TileManager::DidFinishTileInitialization(Tile* tile) {
  ManagedTileState& managed_tile_state = tile->managed_state();
  DCHECK(managed_tile_state.resource);
  managed_tile_state.resource_is_being_initialized = false;
  managed_tile_state.can_be_freed = true;
}

void TileManager::DidTileRasterStateChange(Tile* tile, TileRasterState state) {
  ManagedTileState& mts = tile->managed_state();
  DCHECK_LT(state, NUM_STATES);

  for (int i = 0; i < NUM_TREES; ++i) {
    // Decrement count for current state.
    --raster_state_count_[mts.raster_state][i][mts.tree_bin[i]];
    DCHECK_GE(raster_state_count_[mts.raster_state][i][mts.tree_bin[i]], 0);

    // Increment count for new state.
    ++raster_state_count_[state][i][mts.tree_bin[i]];
  }

  mts.raster_state = state;
}

void TileManager::DidTileBinChange(Tile* tile,
                                   TileManagerBin bin,
                                   WhichTree tree) {
  ManagedTileState& mts = tile->managed_state();

  // Decrement count for current bin.
  --raster_state_count_[mts.raster_state][tree][mts.tree_bin[tree]];
  DCHECK_GE(raster_state_count_[mts.raster_state][tree][mts.tree_bin[tree]], 0);

  // Increment count for new bin.
  ++raster_state_count_[mts.raster_state][tree][bin];

  mts.tree_bin[tree] = bin;
}

// static
void TileManager::RunRasterTask(uint8* buffer,
                                const gfx::Rect& rect,
                                float contents_scale,
                                PicturePileImpl* picture_pile,
                                RenderingStats* stats) {
  TRACE_EVENT0("cc", "TileManager::RunRasterTask");
  DCHECK(picture_pile);
  DCHECK(buffer);
  SkBitmap bitmap;
  bitmap.setConfig(SkBitmap::kARGB_8888_Config, rect.width(), rect.height());
  bitmap.setPixels(buffer);
  SkDevice device(bitmap);
  SkCanvas canvas(&device);
  picture_pile->Raster(&canvas, rect, contents_scale, stats);
}

// static
void TileManager::RunImageDecodeTask(skia::LazyPixelRef* pixel_ref,
                                     RenderingStats* stats) {
  TRACE_EVENT0("cc", "TileManager::RunImageDecodeTask");
  base::TimeTicks decode_begin_time;
  if (stats)
    decode_begin_time = base::TimeTicks::Now();
  pixel_ref->Decode();
  if (stats) {
    stats->totalDeferredImageDecodeCount++;
    stats->totalDeferredImageDecodeTime +=
        base::TimeTicks::Now() - decode_begin_time;
  }
}

}  // namespace cc
