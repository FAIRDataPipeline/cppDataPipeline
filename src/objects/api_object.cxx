#include "fdp/objects/api_object.hxx"
#include "fdp/exceptions.hxx"

namespace FairDataPipeline {

    ApiObject::ApiObject() : obj_(Json::Value() )
    {
    }

    ApiObject::sptr ApiObject::from_json( const Json::Value& j )
    {
        ApiObject::sptr pobj = ApiObject::construct();
        pobj->obj_ = j;

        return pobj;
    }

    ApiObject::sptr ApiObject::construct(void)
    {
        ApiObject::sptr pobj = ApiObject::sptr( new ApiObject() );
        return pobj;
    }

    int ApiObject::add( const std::string& key, int value )
    {
        this->obj_[ key ] = value;
        return 0;
    }
    int ApiObject::add( const std::string& key, float value )
    {
        this->obj_[ key ] = value;
        return 0;
    }

    int ApiObject::add( const std::string& key, double value )
    {
        this->obj_[ key ] = value;
        return 0;
    }

    int ApiObject::add( const std::string& key, const std::string& value )
    {
        this->obj_[ key ] = value;
        return 0;
    }

    int ApiObject::add( const std::string& key, const ApiObject& value )
    {
        this->obj_[ key ] = value.obj_;
        return 0;
    }


    int ApiObject::remove( const std::string& key )
    {
        this->obj_.removeMember( key );
        return 0;
    }


    /**
     * @brief Returns the object id from the object uri_
     * 
     * @return int of the object id
     */
    int ApiObject::get_id() const {
       return get_id_from_string(get_uri());
    }

    int ApiObject::get_id_from_string(std::string url){
        if (url.empty()) {
            throw rest_apiquery_error(
                "Cannot get an object id from an empty url; the registry "
                "may not have returned the expected object");
        }
        if (url.back() == '/')        {
            url = url.substr(0, url.size() - 1);
        }
        std::size_t id_pos = url.find_last_of("/");
        return std::stoi(url.substr(id_pos + 1));
    }
    /**
     * @brief Returns the object type from the object uri_ (table)
     * 
     * @return std::string The object type
     */
    std::string ApiObject::get_type() const {
        std::string uri_ = get_uri();
        std::size_t id_pos = uri_.find_last_of("/");
        std::string uri_prefix = uri_.substr(0, id_pos);
        id_pos = uri_prefix.find_last_of("/");
        return uri_prefix.substr(id_pos + 1);
    }

    std::string ApiObject::get_uri() const{
        return obj_["url"].asString();
    }

    std::string ApiObject::get_value_as_string( const std::string& key) const{
        return obj_[key].asString();
    }
    int ApiObject::get_value_as_int(const std::string& key) const{
        return obj_[key].asInt();
    }

    bool ApiObject::is_empty(){
        if(obj_["url"].asString().empty()){
            return true;
        }
        return false;
    }

    std::string ApiObject::get_first_component() const{
        return obj_["components"][0].asString();
    }
};
