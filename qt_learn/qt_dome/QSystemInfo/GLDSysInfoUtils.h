#ifndef GLDSYSINFOUTILS_H
#define GLDSYSINFOUTILS_H

#include <Windows.h>
#include <string>

    enum WindowsVersion
    {
        Windows,
        Windows32s,
        Windows95,
        Windows95OSR2,
        Windows98,
        Windows98SE,
        WindowsMillennium,
        WindowsNT351,
        WindowsNT40,
        WindowsNT40Server,
        Windows2000,
        WindowsXP,
        WindowsXPProfessionalx64,
        WindowsHomeServer,
        WindowsServer2003,
        WindowsServer2003R2,
        WindowsVista,
        WindowsServer2008,
        WindowsServer2008R2,
        Windows7,
        Windows8,
        WindowsServer2012,
        Windows8Point1,
        WindowsServer2012R2,
        Windows10,
        WindowsServer2016TechnicalPreview
    };

    enum WindowsEdition
    {
        EditionUnknown,                 // 未知版本

        Workstation,                    // 工作站
        Server,                         // 服务版
        AdvancedServer,                 // 高级服务版
        Home,                           // 家庭版

        Ultimate,                       // 旗舰版
        HomeBasic,                      // 家庭普通版
        HomePremium,                    // 家庭高级版
        Enterprise,                     // 企业版
        HomeBasic_N,                    // 家庭普通N版
        Business,                       // 商业版
        StandardServer,                 // 标准服务版
        DatacenterServer,               // 数据中心服务版
        SmallBusinessServer,            // 小商业服务版
        EnterpriseServer,               // 企业服务版
        Starter,                        // 精简版
        DatacenterServerCore,           // 数据中心服务核心版
        StandardServerCore,             // 标准服务核心版
        EnterpriseServerCore,           // 企业服务核心版
        EnterpriseServerIA64,           // 企业服务IA64版
        Business_N,                     // 商用版N
        WebServer,                      // web服务版
        ClusterServer,                  // 集群服务版
        HomeServer,                     // 家庭服务版
        StorageExpressServer,           // 存储便携服务版
        StorageStandardServer,          // 存储标准服务版
        StorageWorkgroupServer,         // 存储工作组服务版
        StorageEnterpriseServer,        // 存储企业服务版
        ServerForSmallBusiness,         // 初级商业服务版
        SmallBusinessServerPremium,     // 初级商业服务高级版
        HomePremium_N,                  // 初级商业服务高级版
        Enterprise_N,                   // 初级商业服务高级版
        Ultimate_N,                     // 初级商业服务高级版
        WebServerCore,                  // 初级商业服务高级版
        MediumBusinessServerManagement, // 初级商业服务高级版
        MediumBusinessServerSecurity,   // 中级商业服务安全版
        MediumBusinessServerMessaging,  // 中级商业服务消息版
        ServerFoundation,               // 服务基站
        HomePremiumServer,              // 家庭高级服务版
        ServerForSmallBusiness_V,       // 初级商业服务V版
        StandardServer_V,               // 标准服务V版
        DatacenterServer_V,             // 数据中心服务V版
        EnterpriseServer_V,             // 企业服务V版
        DatacenterServerCore_V,         // 数据中心服务核心V版
        StandardServerCore_V,           // 标准服务核心V版
        EnterpriseServerCore_V,         // 企业服务核心V版
        HyperV,                         // 超V版
        StorageExpressServerCore,       // 存储初级服务核心版
        StorageStandardServerCore,      // 存储标准服务核心版
        StorageWorkgroupServerCore,     // 存储工作组服务核心版
        StorageEnterpriseServerCore,    // 存储企业服务核心版
        Starter_N,                      // 精简N版
        Professional,                   // 专业版
        Professional_N,                 // 专业N版
        SBSolutionServer,               // 特定方案服务版
        ServerForSBSolution,            // 定制服务版
        StandardServerSolutions,        // 标准服务定制版
        StandardServerSolutionsCore,    // 标准服务定制核心版
        SBSolutionServer_EM,            // 定制服务EM版
        ServerForSBSolution_EM,         // 超V版
        SolutionEmbeddedServer,         // 内嵌版
        SolutionEmbeddedServerCore,     // 内嵌服务核心版
        SmallBusinessServerPremiumCore, // 初级商业服务核心版
        EssentialBusinessServerMGMT,    // 基本商业服务MGMT版
        EssentialBusinessServerADDL,    // 基本商业服务ADDL版
        EssentialBusinessServerMGMTSVC, // 基本商业服务MGMTSVC版
        EssentialBusinessServerADDLSVC, // 基本商业服务ADDLSVC版
        ClusterServer_V,                // 集群服务V版
        Embedded,                       // 超V版
        Starter_E,                      // 精简E版
        HomeBasic_E,                    // 家庭普通E版
        HomePremium_E,                  // 家庭高级E版
        Professional_E,                 // 专业E版
        Enterprise_E,                   // 企业E版
        Ultimate_E                      // 旗舰E版
    };

    /**
    * @brief 获取操作系统版本
    * @return
    */
    WindowsVersion getWindowsVersion();

    /**
    * @brief 获取windows版本
    * @return
    */
    WindowsEdition getWindowsEdition();

    /**
    * @brief 获取平台类型
    * @return
    */
    std::string getPlatformType();

    /**
    * @brief 获取主版本
    * @return
    */
    DWORD getMajorVersion();

    /**
    * @brief 获取次版本号
    * @return
    */
    DWORD getMinorVersion();

    /**
    * @brief 获取Builder Number
    * @return
    */
    DWORD getBuildNumber();

    /**
    * @brief 获取平台ID
    * @return
    */
    DWORD getPlatformID();

    /**
    * @brief 获取Service Pack信息
    * @return
    */
    std::string getServicePackInfo();

    /**
    * @brief 是否是32位平台
    * @return
    */
    bool is32bitPlatform();

    /**
    * @brief 是否是64位平台
    * @return
    */
    bool is64bitPlatform();

    /**
    * @brief 检测是否安装某一软件
    * @param  软件名
    * @return
    */
    bool checkIsInstalled(std::string name);


#endif // GLDSYSINFOUTILS_H
