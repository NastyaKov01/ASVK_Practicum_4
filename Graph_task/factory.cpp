#include "factory.h"

class GraphFactory::TImpl
{
    class ICreator {
    public:
        virtual ~ICreator(){}
        virtual std::unique_ptr<TGraph> Create(std::unique_ptr<Params>&& params) const = 0;
    };
    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;
public:
    template <class TCurrentObject>
    class TCreator : public ICreator{
        std::unique_ptr<TGraph> Create(std::unique_ptr<Params>&& params) const override{
            auto CurParams = dynamic_cast<typename TCurrentObject::ParType*>(params.get());
            params.release();
            return std::make_unique<TCurrentObject>(std::unique_ptr<typename TCurrentObject::ParType>(CurParams));
        }
    };

    TImpl() { RegisterAll();}

    template <typename T>
    void RegisterCreator(const std::string& type) {
        RegisteredCreators[type] = std::make_shared<TCreator<T>>();
    }
    void RegisterAll() {
        RegisterCreator<Bipartite>("bipartite");
        RegisterCreator<Complete>("complete");
        RegisterCreator<Simple>("simple");
        RegisterCreator<Weighted>("weighted");
    }
    std::unique_ptr<TGraph> CreateObject(const std::string& type, std::unique_ptr<Params>&& params) const {
        auto creator = RegisteredCreators.find(type);
        if (creator == RegisteredCreators.end()) {
            return nullptr;
        }
        return creator->second->Create(std::move(params));
    }
};

GraphFactory::GraphFactory() : Impl(std::make_unique<GraphFactory::TImpl>()) {}
GraphFactory::~GraphFactory(){}

std::unique_ptr<TGraph> GraphFactory::Create(const std::string& type, std::unique_ptr<Params>&& params) const
{
    return Impl->CreateObject(type, std::move(params));
}