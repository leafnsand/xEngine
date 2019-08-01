#include "Network/Component/NetworkSystemComponent.h"

#include <AzCore/Socket/AzSocket.h>
#include <AzCore/Script/ScriptSystemBus.h>
#include <AzCore/Script/lua/lua.h>
#include <AzCore/Serialization/SerializeContext.h>

extern "C" int luaopen_protobuf_c(lua_State *L);

namespace Module
{
    void NetworkSystemComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serialize_context = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize_context->Class<NetworkSystemComponent>()
                    ->SerializerForEmptyClass();
        }
    }

    void NetworkSystemComponent::Activate()
    {
        AZ::AzSock::Startup();

        AZ::ScriptContext *context = nullptr;
        EBUS_EVENT_RESULT(context, AZ::ScriptSystemRequestBus, GetContext);

        if (context && context->NativeContext())
        {
            luaL_requiref(context->NativeContext(), "protobuf.c", luaopen_protobuf_c, 0);
            lua_pop(context->NativeContext(), 1);
        }
    }

    void NetworkSystemComponent::Deactivate()
    {
        AZ::AzSock::Cleanup();
    }
}
