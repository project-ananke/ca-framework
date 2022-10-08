#include "styx_renderer.h"

styx_function StyxRenderQueue *
styx_init_render_queue(void)
{
    styx_tctx *tctx = get_tctx();
    StyxRenderQueue *queue = arena_push_struct(tctx->allocator, StyxRenderQueue);
    queue->jobs = 
    (StyxQueueJob *)arena_push_array(tctx->allocator, sizeof(StyxQueueJob), QUEUE_MAX_JOBS);
    queue->job_data = 
    (StyxQueueJobData *)arena_push_array(tctx->allocator, 
                                         sizeof(StyxQueueJobData), QUEUE_MAX_JOBS);
    return queue;
}

styx_function void
styx_render_push(StyxRenderQueue *renderer, 
                 StyxQueueJob job, StyxQueueJobData data)
{
    if (renderer->write_idx == QUEUE_MAX_JOBS - 1) {
        return;
    }
    
    renderer->jobs[renderer->write_idx] = job;
    renderer->job_data[renderer->write_idx] = data;
    
    renderer->write_idx++;
}
