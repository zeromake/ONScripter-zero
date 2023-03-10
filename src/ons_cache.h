#ifndef __ONS_CACHE_H__
#define __ONS_CACHE_H__
#include <string>
#include <memory>
#include <SDL.h>
#include <map>
#include <list>
#include <utility>

namespace onscache {
    template<class Key, class Value>
    class lru_cache
    {
    public:
        typedef Key key_type;
        typedef Value value_type;
        typedef std::list<key_type> list_type;
        typedef std::map<
                    key_type,
                    std::pair<value_type, typename list_type::iterator>
                > map_type;

        lru_cache(size_t capacity)
            : m_capacity(capacity)
        {
        }

        ~lru_cache()
        {
        }

        size_t size() const
        {
            return m_map.size();
        }

        size_t capacity() const
        {
            return m_capacity;
        }

        bool empty() const
        {
            return m_map.empty();
        }

        bool contains(const key_type &key)
        {
            return m_map.find(key) != m_map.end();
        }

        void insert(const key_type &key, const value_type &value)
        {
            typename map_type::iterator i = m_map.find(key);
            if(i == m_map.end()){
                // insert item into the cache, but first check if it is full
                if(size() >= m_capacity){
                    // cache is full, evict the least recently used item
                    evict();
                }

                // insert the new item
                m_list.push_front(key);
                m_map[key] = std::make_pair(value, m_list.begin());
            }
        }

        std::optional<value_type> get(const key_type &key)
        {
            // lookup value in the cache
            typename map_type::iterator i = m_map.find(key);
            if(i == m_map.end()){
                // value not in cache
                return std::nullopt;
            }

            // return the value, but first update its place in the most
            // recently used list
            typename list_type::iterator j = i->second.second;
            if(j != m_list.begin()){
                // move item to the front of the most recently used list
                m_list.erase(j);
                m_list.push_front(key);

                // update iterator in map
                j = m_list.begin();
                const value_type &value = i->second.first;
                m_map[key] = std::make_pair(value, j);

                // return the value
                return value;
            }
            else {
                // the item is already at the front of the most recently
                // used list so just return it
                return i->second.first;
            }
        }

        void clear()
        {
            m_map.clear();
            m_list.clear();
        }

    private:
        void evict()
        {
            // evict item from the end of most recently used list
            typename list_type::iterator i = --m_list.end();
            m_map.erase(*i);
            m_list.erase(i);
        }

    private:
        map_type m_map;
        list_type m_list;
        size_t m_capacity;
    };

    class SurfaceBaseNode {
        public:
            SurfaceBaseNode() {}
            SDL_Surface* v;
            std::optional<bool> has_alpha;
            std::optional<int> location;
            void release();
    };
    class SurfaceUnCacheNode: SurfaceBaseNode {
        public:
            void release() {
                SDL_FreeSurface(v);
                v = nullptr;
            };
            ~SurfaceUnCacheNode() {
                if (v) {
                    SDL_FreeSurface(v);
                    v = nullptr;
                }
            }
    };
    typedef struct SurfaceNode: SurfaceBaseNode {
        void release() {}
        ~SurfaceNode() {
            if (v) {
                SDL_FreeSurface(v);
                v = nullptr;
            }
        }
    } SurfaceNode;
    typedef struct SurfaceCache {
        lru_cache<std::string, std::shared_ptr<SurfaceNode>> store;
        SurfaceCache(size_t size): store(lru_cache<std::string, std::shared_ptr<SurfaceNode>>(size)) {}
        ~SurfaceCache() {}
        std::shared_ptr<SurfaceNode> Put(const std::string &k, SDL_Surface* s) {
            auto node = std::make_shared<SurfaceNode>();
            node->v = s;
            store.insert(k, node);
            return node;
        }
        std::shared_ptr<SurfaceNode> Get(const std::string &k) {
            auto node = store.get(k);
            if (node.has_value()) {
                return node.value();
            }
            return nullptr;
        }
    } SurfaceCache;
}

#endif
