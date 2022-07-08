#include "PlushGame.h"
#include "Modules/ModuleManager.h"
#include "Modules/UI/Converters/CountdownConverter.h"

class FPlushieGameModule: public IModuleInterface
{
public:
	virtual void StartupModule() override
	{
		Noesis::RegisterComponent<CountdownConverter>();
	}
	
	virtual void ShutdownModule() override
	{
		Noesis::UnregisterComponent<CountdownConverter>();
	}
	
	virtual bool IsGameModule() const override
	{
		return true;
	}
};

IMPLEMENT_PRIMARY_GAME_MODULE( FPlushieGameModule, PlushGame, "PlushGame" );
