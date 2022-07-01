////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NoesisSettings.h"

// Core includes
#include "Misc/FileHelper.h"

// CoreUObject includes
#include "Misc/PackageName.h"
#include "UObject/Package.h"

// Engine includes
#include "Engine/FontFace.h"

// FreeType2 includes
#if PLATFORM_COMPILER_HAS_GENERIC_KEYWORD
#define generic __identifier(generic)
#endif	//PLATFORM_COMPILER_HAS_GENERIC_KEYWORD

THIRD_PARTY_INCLUDES_START
#include "ft2build.h"
#include FT_FREETYPE_H
THIRD_PARTY_INCLUDES_END

// Noesis includes
#include "NoesisSDK.h"

// NoesisRuntime includes
#include "NoesisRuntimeModule.h"
#include "NoesisXaml.h"

UNoesisSettings::UNoesisSettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	OffscreenTextureSampleCount = ENoesisOffscreenSampleCount::One;
	GlyphTextureSize = ENoesisGlyphCacheDimensions::x1024;
	ApplicationResources = FSoftObjectPath("/NoesisGUI/Theme/NoesisTheme_DarkBlue.NoesisTheme_DarkBlue");
	DefaultFonts.Add(FSoftObjectPath("/NoesisGUI/Theme/Fonts/PT_Root_UI_Regular.PT_Root_UI_Regular"));
	DefaultFonts.Add(FSoftObjectPath("/NoesisGUI/Theme/Fonts/PT_Root_UI_Bold.PT_Root_UI_Bold"));
	LoadPlatformFonts = true;
	DefaultFontSize = 15.f;
	DefaultFontWeight = ENoesisFontWeight::Normal;
	DefaultFontStretch = ENoesisFontStretch::Normal;
	DefaultFontStyle = ENoesisFontStyle::Normal;
	GeneralLogLevel = ENoesisLoggingSettings::Warning;
	BindingLogLevel = ENoesisLoggingSettings::Warning;
	PremultiplyAlpha = true;
}

void UNoesisSettings::SetLicense() const
{
	Noesis::GUI::SetLicense(TCHAR_TO_UTF8(*LicenseName), TCHAR_TO_UTF8(*LicenseKey));
}

static uint32 ApplicationResourcesHash;
static UNoesisXaml* ApplicationResourcesRef;
void UNoesisSettings::SetApplicationResources() const
{
	// To prevent ApplicationResources and all its dependencies from being deleted
	// during garbage collection we have to manually add it to the root object
	if (ApplicationResourcesRef != nullptr)
	{
		ApplicationResourcesRef->RemoveFromRoot();
	}
	ApplicationResourcesRef = LoadObject<UNoesisXaml>(nullptr, *ApplicationResources.GetAssetPathString(), nullptr, LOAD_NoWarn);
	if (ApplicationResourcesRef)
	{
		ApplicationResourcesRef->AddToRoot();
		uint32 Hash = ApplicationResourcesRef->GetContentHash();
		if (ApplicationResourcesHash != Hash)
		{
			ApplicationResourcesHash = Hash;
			Noesis::Ptr<Noesis::ResourceDictionary> Dictionary = Noesis::MakePtr<Noesis::ResourceDictionary>();
			Noesis::GUI::SetApplicationResources(Dictionary);
			ApplicationResourcesRef->LoadComponent(Dictionary);
		}
	}
	else
	{
		if (ApplicationResourcesHash != 0)
		{
			ApplicationResourcesHash = 0;
			Noesis::GUI::SetApplicationResources(nullptr);
		}
	}
}

static unsigned long StreamRead(FT_Stream Stream, unsigned long Offset, unsigned char* Buffer, unsigned long Count)
{
	FMemory::Memcpy(Buffer, (uint8*)Stream->descriptor.pointer + Offset, Count);
	return Count;
}

static void StreamClose(FT_Stream) {}

static TArray<FString> GetFamilyNames(FT_Library Library, const TArray<uint8>& FontData)
{
	TArray<FString> FamilyNames;

	FT_StreamRec Stream;
	FMemory::Memset(&Stream, 0, sizeof(Stream));
	Stream.descriptor.pointer = (void*)FontData.GetData();
	Stream.size = FontData.Num();
	Stream.read = &StreamRead;
	Stream.close = &StreamClose;

	FT_Open_Args Args;
	FMemory::Memset(&Args, 0, sizeof(Args));
	Args.flags = FT_OPEN_STREAM;
	Args.stream = &Stream;

	FT_Face Face;
	FT_Error Error = FT_Open_Face(Library, &Args, -1, &Face);
	if (Error == 0)
	{
		for (FT_Long FaceIndex = 0; FaceIndex < Face->num_faces; FaceIndex++)
		{
			FT_Face SubFace;
			Error = FT_Open_Face(Library, &Args, FaceIndex, &SubFace);
			if (Error == 0)
			{
				FamilyNames.AddUnique(SubFace->family_name);
				FT_Done_Face(SubFace);
			}
		}
		FT_Done_Face(Face);
	}

	return FamilyNames;
}

static FString GetFamilyName(UFontFace* FontFace, const FString& Name)
{
	UPackage* Package = FontFace->GetOutermost();
	FString PackageRoot, PackagePath, PackageName;
	FPackageName::SplitLongPackageName(Package->GetPathName(), PackageRoot, PackagePath, PackageName, false);
	return PackageRoot.LeftChop(1) + ";component" / PackagePath / "#" + Name;
}

static TArray<UFontFace*> DefaultFontRefs;
void UNoesisSettings::SetFontFallbacks() const
{
	INoesisRuntimeModuleInterface& NoesisRuntime = INoesisRuntimeModuleInterface::Get();
	FT_Library Library;
	FT_Error Error = FT_Init_FreeType(&Library);
	if (Error == 0)
	{
		for (UFontFace* FontFace : DefaultFontRefs)
		{
			FontFace->RemoveFromRoot();
		}
		DefaultFontRefs.Empty(DefaultFonts.Num());
		TArray<Noesis::String> FamilyNamesStr;
		for (auto& FontFallback : DefaultFonts)
		{
			UFontFace* FontFace = Cast<UFontFace>(FontFallback.TryLoad());
			if (FontFace)
			{
				FontFace->AddToRoot();
				DefaultFontRefs.Add(FontFace);
				NoesisRuntime.RegisterFont(FontFace);

#if !WITH_EDITORONLY_DATA
				if (FontFace->GetLoadingPolicy() != EFontLoadingPolicy::Inline)
				{
					TArray<uint8> FileData;
					FFileHelper::LoadFileToArray(FileData, *FontFace->GetFontFilename());

					TArray<FString> Names = GetFamilyNames(Library, FileData);
					for (auto& Name : Names)
					{
						FamilyNamesStr.AddUnique(TCHAR_TO_UTF8(*GetFamilyName(FontFace, Name)));
					}
				}
				else
#endif
				{
					const FFontFaceDataRef FontFaceDataRef = FontFace->FontFaceData;
					const FFontFaceData& FontFaceData = FontFaceDataRef.Get();
					const TArray<uint8>& FontFaceDataArray = FontFaceData.GetData();

					TArray<FString> Names = GetFamilyNames(Library, FontFaceDataArray);
					for (auto& Name : Names)
					{
						FamilyNamesStr.AddUnique(TCHAR_TO_UTF8(*GetFamilyName(FontFace, Name)));
					}
				}
			}
		}

		// Add user font fallbacks
		TArray<const ANSICHAR*> FamilyNames;
		for (auto& Name : FamilyNamesStr)
		{
			FamilyNames.Add(Name.Str());
		}

		// Add platform specific font fallbacks
		if (LoadPlatformFonts)
		{
#if PLATFORM_WINDOWS
			FamilyNames.Add("Arial");
			FamilyNames.Add("Segoe UI Emoji");			// Windows 10 Emojis
			FamilyNames.Add("Arial Unicode MS");		// Almost everything (but part of MS Office, not Windows)
			FamilyNames.Add("Microsoft Sans Serif");	// Unicode scripts excluding Asian scripts
			FamilyNames.Add("Microsoft YaHei");			// Chinese
			FamilyNames.Add("Gulim");					// Korean
			FamilyNames.Add("MS Gothic");				// Japanese
#elif PLATFORM_MAC
			FamilyNames.Add("Arial");
			FamilyNames.Add("Arial Unicode MS");		// MacOS 10.5+
#elif PLATFORM_IOS
			FamilyNames.Add("PingFang SC");				// Simplified Chinese (iOS 9+)
			FamilyNames.Add("Apple SD Gothic Neo");		// Korean (iOS 7+)
			FamilyNames.Add("Hiragino Sans");			// Japanese (iOS 9+)
#elif PLATFORM_ANDROID
			FamilyNames.Add("Noto Sans CJK SC");		// Simplified Chinese
			FamilyNames.Add("Noto Sans CJK KR");		// Korean
			FamilyNames.Add("Noto Sans CJK JP");		// Japanese
#elif PLATFORM_LINUX
			FamilyNames.Add("Noto Sans CJK SC");		// Simplified Chinese
			FamilyNames.Add("Noto Sans CJK KR");		// Korean
			FamilyNames.Add("Noto Sans CJK JP");		// Japanese
#endif
		}

		Noesis::GUI::SetFontFallbacks(FamilyNames.GetData(), FamilyNames.Num());
		FT_Done_FreeType(Library);
	}
}

void UNoesisSettings::SetFontDefaultProperties() const
{
	float Size = DefaultFontSize;
	ENoesisFontWeight Weight = DefaultFontWeight;
	ENoesisFontStretch Stretch = DefaultFontStretch;
	ENoesisFontStyle Style = DefaultFontStyle;
	Noesis::GUI::SetFontDefaultProperties(Size, (Noesis::FontWeight)Weight, (Noesis::FontStretch)Stretch, (Noesis::FontStyle)Style);
}

#if WITH_EDITOR
void UNoesisSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property != NULL)
	{
		FName MemberPropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, LicenseName)
			|| MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, LicenseKey))
		{
			SetLicense();
		}
		else if (MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, ApplicationResources))
		{
			SetApplicationResources();
		}
		else if (MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, DefaultFonts))
		{
			SetFontFallbacks();
		}
		else if (MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, DefaultFontSize)
			|| MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, DefaultFontWeight)
			|| MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, DefaultFontStretch)
			|| MemberPropName == GET_MEMBER_NAME_CHECKED(UNoesisSettings, DefaultFontStyle))
		{
			SetFontDefaultProperties();
		}
	}
}
#endif
