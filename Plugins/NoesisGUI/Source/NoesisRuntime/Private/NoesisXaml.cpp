////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NoesisXaml.h"

// CoreUObject includes
#include "Misc/PackageName.h"

// Engine includes
#include "EditorFramework/AssetImportData.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"

// NoesisRuntime includes
#include "NoesisInstance.h"
#include "NoesisRuntimeModule.h"

UNoesisXaml::UNoesisXaml(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

static FString GetXamlUri(UObject* Package)
{
	FString PackageRoot, PackagePath, PackageName;
	FPackageName::SplitLongPackageName(Package->GetPathName(), PackageRoot, PackagePath, PackageName, false);
	return PackageRoot.LeftChop(1) + TEXT(";component/") + PackagePath + PackageName + TEXT(".xaml");
}

Noesis::Ptr<Noesis::BaseComponent> UNoesisXaml::LoadXaml()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
		return nullptr;

	FString Uri = GetXamlUri(GetOutermost());
	return Noesis::GUI::LoadXaml(TCHAR_TO_UTF8(*Uri));
}

void UNoesisXaml::LoadComponent(Noesis::BaseComponent* Component)
{
	FString Uri = GetXamlUri(GetOutermost());
	Noesis::GUI::LoadComponent(Component, TCHAR_TO_UTF8(*Uri));
}

uint32 UNoesisXaml::GetContentHash() const
{
	uint32 Hash = Noesis::HashBytes(XamlText.GetData(), XamlText.Num());

	for (auto Xaml : Xamls)
	{
		if (Xaml != nullptr)
		{
			Hash ^= Xaml->GetContentHash();
		}
	}

	return Hash;
}

#if WITH_EDITORONLY_DATA
void UNoesisXaml::PostInitProperties()
{
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		AssetImportData = NewObject<UAssetImportData>(this, TEXT("AssetImportData"));
	}

	Super::PostInitProperties();
}

void UNoesisXaml::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
{
	if (AssetImportData)
	{
		OutTags.Add(FAssetRegistryTag(SourceFileTagName(), AssetImportData->GetSourceData().ToJson(), FAssetRegistryTag::TT_Hidden));
	}

	Super::GetAssetRegistryTags(OutTags);
}
#endif // WITH_EDITORONLY_DATA

void UNoesisXaml::RegisterDependencies()
{
	INoesisRuntimeModuleInterface& NoesisRuntime = INoesisRuntimeModuleInterface::Get();
	for (auto Font : Fonts)
	{
		for (auto TypefaceEntry : Font->CompositeFont.DefaultTypeface.Fonts)
		{
			const FFontData* FontData = &TypefaceEntry.Font;
			const UFontFace* FontFace = Cast<UFontFace>(FontData->GetFontFaceAsset());
			NoesisRuntime.RegisterFont(FontFace);
		}
	}

	for (auto FontFace : FontFaces)
	{
		NoesisRuntime.RegisterFont(FontFace);
	}
}

#if WITH_EDITOR
bool UNoesisXaml::CanRenderThumbnail()
{
	if (!ThumbnailRenderInstance)
	{
		ThumbnailRenderInstance = NewObject<UNoesisInstance>();
		ThumbnailRenderInstance->BaseXaml = this;
		ThumbnailRenderInstance->InitInstance();
	}

	return ThumbnailRenderInstance ? ThumbnailRenderInstance->XamlView != nullptr : false;
}

void UNoesisXaml::RenderThumbnail(FIntRect ViewportRect, const FTexture2DRHIRef& BackBuffer)
{
	if (!ThumbnailRenderInstance)
	{
		ThumbnailRenderInstance = NewObject<UNoesisInstance>();
		ThumbnailRenderInstance->BaseXaml = this;
		ThumbnailRenderInstance->InitInstance();
	}

	if (ThumbnailRenderInstance)
	{
		ThumbnailRenderInstance->DrawThumbnail(ViewportRect, BackBuffer);
	}
}

void UNoesisXaml::DestroyThumbnailRenderData()
{
	if (ThumbnailRenderInstance)
	{
		ThumbnailRenderInstance->TermInstance();
		ThumbnailRenderInstance = nullptr;
	}
}
#endif
