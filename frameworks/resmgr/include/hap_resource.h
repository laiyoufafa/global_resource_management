/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef RESOURCE_MANAGER_HAPRESOURCE_H
#define RESOURCE_MANAGER_HAPRESOURCE_H

#include <map>
#include <string>
#include <time.h>
#include <unordered_map>
#include <vector>
#include "res_desc.h"
#include "res_config_impl.h"

namespace OHOS {
namespace Global {
namespace Resource {
/**
 * HapResource describe a resource of hap zip file.
 *
 */
class HapResource {
public:
    /**
     * Creates an HapResource.
     *
     * @param path resources.index file path
     * @param defaultConfig  match defaultConfig to keys of index file, only parse the matched keys.
     *                       'null' means parse all keys.
     * @param system If `system` is true, the package is marked as a system package, and allows some functions to
     *               filter out this package when computing what configurations/resources are available.
     * @return
     */
    static const HapResource *LoadFromIndex(const char *path, const ResConfigImpl *defaultConfig, bool system = false);

    static const std::unordered_map<std::string, HapResource *> LoadOverlays(const std::string &path,
        const std::vector<std::string> &overlayPath, const ResConfigImpl *defaultConfig);

    ~HapResource();

    inline const std::string GetIndexPath() const
    {
        return indexPath_;
    }

    inline const std::string GetResourcePath() const
    {
        return resourcePath_;
    }

    const std::vector<std::string> GetQualifiers() const;

    /**
     * describe limitpath and value under the path
     */
    class ValueUnderQualifierDir {
    public:
        inline const std::vector<KeyParam *> GetKeyParams() const
        {
            return keyParams_;
        }

        inline const std::string GetFolder() const
        {
            return folder_;
        }

        inline const IdItem *GetIdItem() const
        {
            return idItem_;
        }

        inline const ResConfigImpl *GetResConfig() const
        {
            return resConfig_;
        }

        inline const HapResource *GetHapResource() const
        {
            return hapResource_;
        }

        inline bool IsOverlay() const
        {
            return isOverlay_;
        }

        ValueUnderQualifierDir(const std::vector<KeyParam *> &keyParams, IdItem *idItem,
            HapResource *hapResource, bool isOverlay = false);

        ~ValueUnderQualifierDir();

    private:

        // using keyParams_ to init resconfig_
        void InitResConfig();

        /*
         * keyParams_, folder_, resConfig_ are 3 different ways to describe Qualifiers Sub-directory
         */
        std::vector<KeyParam *> keyParams_;
        // the qualifier path name
        std::string folder_;
        // ResConfig
        ResConfigImpl *resConfig_;

        // the value
        IdItem *idItem_;

        // indicate belong to which hapresource
        const HapResource *hapResource_;

        friend class HapResource;

        bool isOverlay_;
    };

    /**
     * describe value under different Qualifiers Sub-directories
     */
    class IdValues {
    public:
        inline void AddLimitPath(ValueUnderQualifierDir *vuqd)
        {
            limitPaths_.push_back(vuqd);
        }

        inline const std::vector<ValueUnderQualifierDir *> &GetLimitPathsConst() const
        {
            return limitPaths_;
        }

        ~IdValues();

    private:
        // the folder desc
        std::vector<ValueUnderQualifierDir *> limitPaths_;
    };

    const IdValues *GetIdValues(const uint32_t id) const;

    const IdValues *GetIdValuesByName(const std::string name, const ResType resType) const;

    int GetIdByName(const char *name, const ResType resType) const;

    size_t IdSize() const
    {
        return idValuesMap_.size();
    }

private:
    HapResource(const std::string path, time_t lastModTime, const ResConfig *defaultConfig, ResDesc *resDes);

    std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> BuildNameTypeIdMapping() const;

    void UpdateOverlayInfo(std::unordered_map<std::string, std::unordered_map<ResType, uint32_t>> &nameTypeId);

    // must call Init() after constructor
    bool Init();

    // step of Init(), called in Init()
    bool InitIdList();

    // resources.index file path
    const std::string indexPath_;

    // resource path , calculated from indexPath_
    std::string resourcePath_;

    // last mod time of hap file
    time_t lastModTime_;

    // resource information stored in resDesc_
    ResDesc *resDesc_;

    std::map<uint32_t, IdValues *> idValuesMap_;

    // the key is name, each restype holds one map
    // name may conflict in same restype !
    std::vector<std::map<std::string, IdValues *> *> idValuesNameMap_;

    // default resconfig
    const ResConfig *defaultConfig_;
};
} // namespace Resource
} // namespace Global
} // namespace OHOS
#endif
