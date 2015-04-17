
// albers specific includes
#include "albers/IReader.h"
#include "albers/CollectionBase.h"
#include "albers/EventStore.h"

namespace albers {

  EventStore::EventStore() :
    m_reader(nullptr),
    m_table(new CollectionIDTable())
  {}

  EventStore::~EventStore(){
    for (auto& coll : m_collections){
      delete coll.second;
    }
  }

  bool EventStore::get(int id, CollectionBase*& collection) const{
    auto val = m_retrievedIDs.insert(id);
    bool success = false;
    if (val.second == true){
      // collection not yet retrieved in recursive-call
      auto name = m_table->name(id);
      success = doGet(name, collection,true);
    } else {
      // collection already requested in recursive call
      // do not set the references to break collection dependency-cycle
      auto name = m_table->name(id);
      success = doGet(name, collection,false);
    }
    m_retrievedIDs.erase(id);
    return success;
  }

  bool EventStore::doGet(const std::string& name, CollectionBase*& collection, bool setReferences) const {
    auto result = std::find_if(begin(m_collections), end(m_collections),
                               [name](const CollPair& item)->bool { return name==item.first; }
    );
    if (result != end(m_collections)){
      auto tmp = result->second;
      if (tmp != nullptr){
        collection = tmp;
        return true;
      }
    } else if (m_reader != nullptr) {
      auto tmp = m_reader->readCollection(name);
      if (setReferences == true) {
        tmp->setReferences(this);
        if (tmp != nullptr){
          // check again whether collection exists
          // it may have been created on-demand already
          if (collectionRegistered(name) == false) {
            m_collections.emplace_back(std::make_pair(name,tmp));
          }
        }
      }
      collection = tmp;
      if (tmp != nullptr) return true;
    } else {
      return false;
    }
    return false;
  }

  void EventStore::clearCollections(){
    for (auto& coll : m_collections){
      coll.second->clear();
    }
  }

  void EventStore::clear(){
    for (auto& coll : m_collections){
      coll.second->clear();
      delete coll.second;
    }
    m_collections.clear();
  }

  bool EventStore::collectionRegistered(const std::string& name) const {
    auto result = std::find_if(begin(m_collections), end(m_collections),
                               [name](const CollPair& item)->bool { return name==item.first; }
    );
    return (result != end(m_collections));
  }

  void EventStore::setReader(IReader* reader){
    m_reader = reader;
    setCollectionIDTable(reader->getCollectionIDTable());
  }


} // namespace
