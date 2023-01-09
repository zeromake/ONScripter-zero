#include "ons_cache.h"
#include "private/utils.h"


namespace onscache {
    void SurfaceUnCacheNode::release() {
        if (v) {
            SDL_FreeSurface(v);
            v = nullptr;
        }
    }
    SurfaceUnCacheNode::SurfaceUnCacheNode() {}
    SurfaceUnCacheNode::~SurfaceUnCacheNode() {
        release();
    }


    void SurfaceNode::release() {}
    SurfaceNode::SurfaceNode() {}
    SurfaceNode::~SurfaceNode() {
        if (v) {
            SDL_FreeSurface(v);
            v = nullptr;
        }
    }
}