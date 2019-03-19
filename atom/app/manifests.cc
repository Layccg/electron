// Copyright (c) 2019 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "atom/app/manifests.h"

#include "base/no_destructor.h"
#include "content/public/common/service_names.mojom.h"
#include "printing/buildflags/buildflags.h"
#include "services/proxy_resolver/public/cpp/manifest.h"
#include "services/service_manager/public/cpp/manifest_builder.h"

#if BUILDFLAG(ENABLE_PRINTING)
#include "components/services/pdf_compositor/public/cpp/manifest.h"
#endif

#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
#include "chrome/services/printing/public/cpp/manifest.h"
#endif

#include <set>

const service_manager::Manifest& GetElectronContentBrowserOverlayManifest() {
  static base::NoDestructor<service_manager::Manifest> manifest{
      service_manager::ManifestBuilder()
          .WithDisplayName("Electron (browser process)")
          .RequireCapability("device", "device:geolocation_control")
          .RequireCapability("proxy_resolver", "factory")
          .RequireCapability("chrome_printing", "converter")
          .RequireCapability("pdf_compositor", "compositor")
          .RequireCapability(content::mojom::kRendererServiceName,
                             "electron_api")
          .Build()};
  return *manifest;
}

const service_manager::Manifest& GetElectronContentRendererOverlayManifest() {
  static base::NoDestructor<service_manager::Manifest> manifest{
      service_manager::ManifestBuilder()
          .WithDisplayName("Electron (renderer process)")
          .ExposeCapability("electron_api",
                            std::set<const char*>{
                                "electron_api.mojom.Electron",
                            })
          .ExposeInterfaceFilterCapability_Deprecated(
              "navigation:frame", "browser",
              std::set<const char*>{
                  "electron_api.mojom.Electron",
              })
          .Build()};
  return *manifest;
}

const std::vector<service_manager::Manifest>&
GetElectronPackagedServicesOverlayManifest() {
  static base::NoDestructor<std::vector<service_manager::Manifest>> manifests{{
      proxy_resolver::GetManifest(),
#if BUILDFLAG(ENABLE_PRINTING)
      printing::GetPdfCompositorManifest(),
#endif
#if BUILDFLAG(ENABLE_PRINT_PREVIEW)
      GetChromePrintingManifest(),
#endif
  }};
  return *manifests;
}