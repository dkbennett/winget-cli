// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once
#include <AppInstallerVersions.h>
#include <dwrite_3.h>
#include <wil/com.h>
#include <wil/registry.h>
#include <winget/Manifest.h>
#include <winget/ManifestCommon.h>

namespace AppInstaller::Fonts
{
    enum class InstallerSource
    {
        // Installer source is not guaranteed to be absolutely correct, as it is just
        // filesystem and registry. We make assumptions based on the structure of how
        // the font is installed to make a best-effort determination.
        Unknown,    // Can be any external font installer
        WinGet,     // Font installed by WinGet
    };

    enum class FontStatus
    {
        // The current status of a particular font file / package.
        Unknown,
        Absent,     // Font is not present at all.
        OK,         // Font is present and in a good state.
        Corrupt,    // Font is partially installed (has a file, but no registry, or vice-versa)
    };

    enum class FontResult
    {
        Unknown,
        Success,
        Error,
    };

    struct FontFace
    {
        std::wstring Name;
        std::vector<std::filesystem::path> FilePaths;
        Utility::OpenTypeFontVersion Version;
    };

    struct FontFamily
    {
        std::wstring Name;
        std::vector<FontFace> Faces;
    };

    // Represents information about a font file used for its installation, query, and removal.
    struct FontFileInfo
    {
        std::filesystem::path FilePath;         // Where the file currently is
        std::filesystem::path InstallPath;      // Where the file should be if installed.
        std::wstring Title;
        Manifest::ScopeEnum Scope = Manifest::ScopeEnum::Unknown;
        DWRITE_FONT_FILE_TYPE FileType = DWRITE_FONT_FILE_TYPE_UNKNOWN;
        InstallerSource InstallerSource = InstallerSource::Unknown;
        FontStatus Status = FontStatus::Unknown;
        bool WinGetSupported = false;
        bool IsFontFileInstalled = false;
        bool IsFontFileRegistered = false;
        std::wstring PackageId;
        std::wstring PackageVersion;

        // Fonts that are not winget-packaged will use this for an identifier.
        std::wstring PackageIdentifier;

        // Registry Install path is where the installation with the system is located.
        std::wstring RegistryInstallPath;

        // Registry Source path is where package mapping of the package is.
        std::wstring RegistryPackagePath;
    };

    // Represents information about a WinGet-installed font package.
    struct FontPackageInfo
    {
        Manifest::ScopeEnum Scope = Manifest::ScopeEnum::Unknown;
        FontStatus Status = FontStatus::Unknown;
        std::wstring PackageId;
        std::wstring PackageVersion;
        std::wstring PackageIdentifier;
    };

    struct FontContext
    {
        InstallerSource InstallerSource = InstallerSource::Unknown;
        Manifest::ScopeEnum Scope = Manifest::ScopeEnum::Unknown;
        std::vector<std::filesystem::path> PackageFiles = std::vector<std::filesystem::path>();
        std::wstring PackageId;
        std::wstring PackageVersion;
        bool Force = false;

        void AddPackageFile(const std::filesystem::path& filePath);
        Manifest::AppsAndFeaturesEntry GetAppsAndFeaturesEntry() const;
        std::wstring GetPackageIdentifier() const;
    };

    struct FontValidationResult
    {
        FontResult Result = FontResult::Unknown;
        FontStatus Status = FontStatus::Unknown;
        bool HasUnsupportedFonts = false;
        winrt::hresult HResult = winrt::hresult(S_OK);
        std::vector<FontFileInfo> FontFileInfos = std::vector<FontFileInfo>();
    };

    struct FontOperationResult
    {
        winrt::hresult HResult = winrt::hresult(S_OK);
        FontResult Result() const;

        // TODO: Add optional rollback context to unwind installs if one operation succeeded but a subsequent one failed.
    };

    std::vector<FontFileInfo> GetInstalledFontFiles();

    std::vector<FontPackageInfo> GetInstalledFontPackages(Manifest::ScopeEnum scope);

    FontFileInfo CreateFontFileInfo(const FontContext& context, const std::filesystem::path& filePath, const std::wstring& title = std::wstring());

    FontValidationResult ValidateFontPackage(FontContext& context);

    FontOperationResult InstallFontPackage(FontContext& context);

    FontOperationResult UninstallFontPackage(FontContext& context);

    std::wstring GetFontRegistryRoot();

    struct FontCatalog
    {
        FontCatalog();

        // Gets all installed font families on the system. If an exact family name is provided and found, returns the installed font family.
        std::vector<FontFamily> GetInstalledFontFamilies(std::optional<std::wstring> familyName = {});

        // Returns a boolean value indicating whether the specified file path is a valid font file.
        bool IsFontFileSupported(const std::filesystem::path& filePath, DWRITE_FONT_FILE_TYPE& fileType);

        std::filesystem::path GetRootFontPath(Manifest::ScopeEnum scope);

    private:
        FontFamily GetFontFamilyByIndex(const wil::com_ptr<IDWriteFontCollection>& collection, UINT32 index);
        std::wstring GetLocalizedStringFromFont(const wil::com_ptr<IDWriteLocalizedStrings>& localizedStringCollection);
        std::wstring GetFontFamilyName(const wil::com_ptr<IDWriteFontFamily>& fontFamily);
        std::wstring GetFontFaceName(const wil::com_ptr<IDWriteFont>& font);
        Utility::OpenTypeFontVersion GetFontFaceVersion(const wil::com_ptr<IDWriteFont>& font);

        wil::com_ptr<IDWriteFactory7> m_factory;
        std::vector<std::wstring> m_preferredLocales;
    };
}
