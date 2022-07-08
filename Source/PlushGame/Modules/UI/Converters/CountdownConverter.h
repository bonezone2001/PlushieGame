#pragma once

#include "NoesisRuntime.h"

/**
 * 
 */
class PLUSHGAME_API CountdownConverter : public Noesis::BaseValueConverter
{
public:
	bool TryConvert(Noesis::BaseComponent* value, const Noesis::Type* targetType, Noesis::BaseComponent*, Noesis::Ptr<Noesis::BaseComponent>& result)
	{
		if (targetType == Noesis::TypeOf<Noesis::String>() && Noesis::Boxing::CanUnbox<int32>(value))
		{
			int32 countdown = Noesis::Boxing::Unbox<int32>(value);
			Noesis::String returnVal = "";
			if (countdown >= 100)
				returnVal.AppendFormat("WAITING FOR PLAYERS");
			else if (countdown > 0)
				returnVal.AppendFormat("%i", countdown);
			else
				returnVal.AppendFormat("START");
			result = Noesis::Boxing::Box(returnVal);
			return true;
		}
		
		return false;
	}

private:
	NS_IMPLEMENT_INLINE_REFLECTION_(CountdownConverter, Noesis::BaseValueConverter, "UI.HUD.CountdownConverter")
};