////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NoesisResourceProvider.h"

// Core includes
#include "Misc/FileHelper.h"

// Engine includes
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"

// NoesisRuntime includes
#include "NoesisXaml.h"
#include "NoesisSupport.h"

void FNoesisXamlProvider::OnXamlChanged(UNoesisXaml* Xaml)
{
#if WITH_EDITOR
	if (FString* Name = NameMap.Find(Xaml))
	{
		RaiseXamlChanged(TCHAR_TO_UTF8(**Name));
	}
#endif
}

Noesis::Ptr<Noesis::Stream> FNoesisXamlProvider::LoadXaml(const Noesis::Uri& Uri)
{
	FString XamlPath = NsProviderUriToAssetPath(Uri);
	UNoesisXaml* Xaml = LoadObject<UNoesisXaml>(nullptr, *XamlPath, nullptr, LOAD_NoWarn);

	if (Xaml)
	{
		// Need to register the XAML's dependencies before returning.
		Xaml->RegisterDependencies();

#if WITH_EDITOR
		NameMap.Add(Xaml, Uri.Str());
#endif

		return *new Noesis::MemoryStream(Xaml->XamlText.GetData(), (uint32)Xaml->XamlText.Num());
	}

	return nullptr;
};

void FNoesisTextureProvider::OnTextureChanged(UTexture2D* Texture)
{
#if WITH_EDITOR
	if (FString* Name = NameMap.Find(Texture))
	{
		RaiseTextureChanged(TCHAR_TO_UTF8(**Name));
	}
#endif
}

Noesis::TextureInfo FNoesisTextureProvider::GetTextureInfo(const Noesis::Uri& Uri)
{
	FString TexturePath = NsProviderUriToAssetPath(Uri);
	UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *TexturePath, nullptr, LOAD_NoWarn);

	if (Texture)
	{
#if WITH_EDITOR
		NameMap.Add(Texture, Uri.Str());
#endif
		return Noesis::TextureInfo { (uint32)Texture->GetSizeX(), (uint32)Texture->GetSizeY() };
	}

	return Noesis::TextureInfo {};
}

Noesis::Ptr<Noesis::Texture> FNoesisTextureProvider::LoadTexture(const Noesis::Uri& Uri, Noesis::RenderDevice* RenderDevice)
{
	FString TexturePath = NsProviderUriToAssetPath(Uri);
	FString TextureObjectPath = TexturePath + TEXT(".") + FPackageName::GetShortName(TexturePath);
	UTexture2D* Texture = FindObject<UTexture2D>(nullptr, *TextureObjectPath);

	if (Texture)
	{
		return NoesisCreateTexture(Texture);
	}

	return nullptr;
};

void FNoesisFontProvider::RegisterFont(const UFontFace* FontFace)
{
	if (FontFace != nullptr)
	{
		UObject* Package = FontFace->GetOutermost();
		FString PackageRoot, PackagePath, PackageName;
		FPackageName::SplitLongPackageName(Package->GetPathName(), PackageRoot, PackagePath, PackageName, false);

		FString Folder = PackageRoot.LeftChop(1) + TEXT(";component/") + PackagePath;

		Noesis::CachedFontProvider::RegisterFont(TCHARToNsString(*Folder).Str(), TCHARToNsString(*PackageName).Str());
	}
}

static Noesis::Uri GetUnrealUri(const Noesis::Uri& Uri)
{
	Noesis::FixedString<512> Path;
	Uri.GetPath(Path);

	Noesis::String Assembly;
	Uri.GetAssembly(Assembly);

	Noesis::FixedString<512> UnrealUri;
	UnrealUri += "/";
	UnrealUri += GetAssetRoot(Assembly).Str();
	UnrealUri += ";component/";
	UnrealUri += Path.Str();

	return UnrealUri.Str();
}

Noesis::FontSource FNoesisFontProvider::MatchFont(const Noesis::Uri& BaseUri, const char* FamilyName, Noesis::FontWeight& Weight,
	Noesis::FontStretch& Stretch, Noesis::FontStyle& Style)
{
	return Noesis::CachedFontProvider::MatchFont(GetUnrealUri(BaseUri), FamilyName, Weight, Stretch, Style);
}

bool FNoesisFontProvider::FamilyExists(const Noesis::Uri& BaseUri, const char* FamilyName)
{
	return Noesis::CachedFontProvider::FamilyExists(GetUnrealUri(BaseUri), FamilyName);
}

void FNoesisFontProvider::ScanFolder(const Noesis::Uri& InFolder)
{
}

Noesis::Ptr<Noesis::Stream> FNoesisFontProvider::OpenFont(const Noesis::Uri& InFolder, const char* InFilename) const
{
	Noesis::FixedString<512> Uri = InFolder.Str();
	Uri += "/";
	Uri += InFilename;

	FString FontPath = NsProviderUriToAssetPath(Uri.Str());
	const UFontFace* FontFace = LoadObject<UFontFace>(nullptr, *FontPath, nullptr, LOAD_NoWarn);

	if (FontFace != nullptr)
	{
		class FontArrayMemoryStream : public Noesis::MemoryStream
		{
		public:
			FontArrayMemoryStream(TArray<uint8>&& InFileData)
				: Noesis::MemoryStream(InFileData.GetData(), (uint32)InFileData.Num()),
				FileData(MoveTemp(InFileData))
			{
			}

		private:
			TArray<uint8> FileData;
		};
#if !WITH_EDITORONLY_DATA
		if (FontFace->GetLoadingPolicy() != EFontLoadingPolicy::Inline)
		{
			TArray<uint8> FileData;
			FFileHelper::LoadFileToArray(FileData, *FontFace->GetFontFilename());
			return *new FontArrayMemoryStream(MoveTemp(FileData));
		}
		else
#endif
		{
			const FFontFaceDataRef FontFaceDataRef = FontFace->FontFaceData;
			const FFontFaceData& FontFaceData = FontFaceDataRef.Get();
			const TArray<uint8>& FontFaceDataArray = FontFaceData.GetData();
			return *new FontArrayMemoryStream(CopyTemp(FontFaceDataArray));
		}
	}

	return nullptr;
}
