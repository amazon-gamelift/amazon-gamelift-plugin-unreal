// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.

#pragma once

// Standard Library
#include <fstream>
#include <iostream>
#include <regex>

// AWS SDK
#include <aws/core/utils/base64/Base64.h>
#include <aws/cloudformation/CloudFormationClient.h>
#include <aws/cloudformation/model/CreateStackRequest.h>
#include <aws/cloudformation/model/DeleteStackRequest.h>
#include <aws/cloudformation/model/DescribeStackEventsRequest.h>
#include <aws/cloudformation/model/DescribeStackResourcesRequest.h>
#include <aws/cloudformation/model/DescribeStacksRequest.h>
#include <aws/cloudformation/model/GetTemplateRequest.h>
#include <aws/cloudformation/model/StackStatus.h>
#include <aws/cloudformation/model/UpdateStackRequest.h>
#include <aws/cloudformation/model/CancelUpdateStackRequest.h>
#include <aws/lambda/LambdaClient.h>
#include <aws/lambda/model/DeleteLayerVersionRequest.h>
#include <aws/lambda/model/PublishLayerVersionRequest.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/ssm/SSMClient.h>
#include <aws/ssm/model/GetParameterRequest.h>
#include <aws/ssm/model/PutParameterRequest.h>

// GameLift
#include <aws/gamelift/core/awsclients/api_initializer.h>
#include <aws/gamelift/core/awsclients/default_clients.h>
#include <aws/gamelift/core/enums.h>
#include <aws/gamelift/core/feature_resources_callback.h>
#include <aws/gamelift/core/gamelift_feature.h>
#include <aws/gamelift/core/model/account_credentials.h>
#include <aws/gamelift/core/model/account_info.h>
#include <aws/gamelift/core/model/config_consts.h>
#include <aws/gamelift/core/model/template_consts.h>
#include <aws/gamelift/core/paramstore_keys.h>
#include <aws/gamelift/core/zipper.h>
#include <aws/gamelift/core/utils/file_utils.h>
#include <aws/gamelift/core/aws_region_mappings.h>

// yaml-cpp
#include <yaml-cpp/yaml.h>

namespace GameLift
{
    /**
     * GameLiftFeatureResources offers methods for working on the AWS resources of a single GAMELIFT feature (ex: "achievements").
     *
     * Each instance of this class operates on one feature, which is specified in it's constructor.
     *
     * For example, it can deploy/delete the feature's CloudFormation stack and create it's instance template files for deployment.
     */
    class GAMELIFT_API GameLiftFeatureResources
    {
    private:
        AccountInfoCopy m_accountInfo;
        AccountCredentialsCopy m_credentials;
        FeatureType m_featureType;
        FuncLogCallback m_logCb = nullptr;

        Aws::S3::S3Client* m_s3Client;
        Aws::SSM::SSMClient* m_ssmClient;
        Aws::CloudFormation::CloudFormationClient* m_cfClient;
        Aws::Lambda::LambdaClient* m_lambdaClient;

        bool m_isUsingSharedS3Client = false;
        bool m_isUsingSharedSSMClient = false;
        bool m_isUsingSharedCfClient = false;
        bool m_isUsingSharedLambdaClient = false;

        std::string m_stackName;
        std::string m_layersReplacementId;
        std::string m_functionsReplacementId;

        std::string m_pluginRoot;
        std::string m_gameliftRoot;
        std::string m_baseLayersPath;
        std::string m_baseFunctionsPath;
        std::string m_baseCloudformationPath;
        std::string m_baseConfigOutputsPath;
        std::string m_instanceLayersPath;
        std::string m_instanceFunctionsPath;
        std::string m_instanceCloudformationPath;

        std::unordered_map<std::string, bool> m_resouceStatusMap;

        Aws::Vector<Aws::CloudFormation::Model::Parameter> getStackParameters(TemplateType templateType) const;
        std::string getRawStackParameters(TemplateType templateType) const;
        std::string getFeatureDashboardTemplate(TemplateType templateType) const;
        std::string getCloudFormationTemplate(TemplateType templateType) const;
        unsigned int createStack() const;
        unsigned int updateStack() const;
        unsigned int cancelUpdateStack() const;
        unsigned int deleteStack() const;
        Aws::CloudFormation::Model::StackStatus periodicallyDescribeStackEvents();
        void describeStackEvents();
        unsigned int getDeployedTemplateBody(const std::string stackName, std::string& templateBody) const;
        bool isTerminalState(Aws::CloudFormation::Model::StackStatus status);
        bool isFailedState(Aws::CloudFormation::Model::StackStatus status);
        bool isFailedResourceStatus(Aws::CloudFormation::Model::ResourceStatus status);
        bool isCreateOrUpdateInProgressState(Aws::CloudFormation::Model::StackStatus status);
        std::string getTempLayersPath() const;
        std::string getTempFunctionsPath() const;
        std::string getTempGameServerPath() const;
        std::string MakeGameServerTempZipPath(const std::string& gameServerPath, const std::string& gameServerHash) const;
        YAML::Node getClientConfigYaml() const;
        Aws::Vector<std::tuple<std::string, std::string>> getConfigOutputParameters() const;
        unsigned int writeCloudFormationParameterInstance(const std::string& cfParams) const;
        unsigned int writeCloudFormationTemplateInstance(const std::string& cfParams) const;
        unsigned int writeCloudFormationDashboardInstance(const std::string& cfTemplate) const;
        unsigned int writeClientConfigYamlToDisk(const YAML::Node& paramsYml) const;
        unsigned int removeOutputsFromClientConfiguration() const;
        unsigned int writeClientConfigurationWithOutputs(Aws::Vector<Aws::CloudFormation::Model::Output> outputs) const;
        std::string getFeatureLayerNameFromDirName(const std::string& layerDirName) const;
        Aws::Lambda::Model::PublishLayerVersionOutcome createFeatureLayer(const std::string& layerDirName, const std::string& s3ObjectName);
        bool lambdaLayerHashChanged(const std::string& layerName, const std::string& layerHash) const;
        std::pair<bool, std::string> gameServerHashChanged(const std::string& gameServerName, const std::string& gameServerHash) const;
        std::pair<bool, std::string> gameServerAddonsHashChanged(const std::string& gameServerName, const std::string& gameServerAddonsHash) const;
        std::pair<bool, std::string> paramHashChanged(const std::string& paramName, const std::string& paramHash) const;
        unsigned int createAndSetLambdaLayerHash(const std::string& layerName, const std::string& layerHash) const;
        unsigned int createAndSetLambdaLayerArn(const std::string& layerName, const std::string& layerArn) const;
        unsigned int createAndSetGameServerHash(const std::string& gameServerName, const std::string& gameServerHash) const;
        unsigned int createAndSetGameServerAddonsHash(const std::string& gameServerName, const std::string& gameServerAddonsHash) const;
        unsigned int createAndSetParamHash(const std::string& paramName, const std::string& paramHash) const;
        static std::string createSanitizedResourceName(const std::string& originalName);
        std::string getShortRegionCode();

    public:
        GameLiftFeatureResources(const AccountInfo accountInfo, const AccountCredentials credentials, FeatureType featureType, FuncLogCallback logCb);
        GameLiftFeatureResources(const AccountInfoCopy& accountInfo, const AccountCredentialsCopy& credentials, FeatureType featureType, FuncLogCallback logCb);
        ~GameLiftFeatureResources();

        // Clients initialized with his method will be deleted in the class destructor.
        void InitializeDefaultAwsClients();

        // Returns Account Info
        AccountInfoCopy GetAccountInfo()
        {
            return m_accountInfo;
        }

        // Returns Account Credentials
        AccountCredentialsCopy GetAccountCredentials()
        {
            return m_credentials;
        }

        // Sets the root directory of the plugin's installation
        inline void SetPluginRoot(const std::string& pluginRoot)
        {
            m_pluginRoot = pluginRoot;
            m_baseLayersPath = pluginRoot + ResourceDirectories::LAYERS_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
            m_baseFunctionsPath = pluginRoot + ResourceDirectories::FUNCTIONS_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
            m_baseCloudformationPath = pluginRoot + ResourceDirectories::CLOUDFORMATION_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
            m_baseConfigOutputsPath = pluginRoot + ResourceDirectories::CONFIG_OUTPUTS_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
        }

        // Returns the root directory of the plugin's installation
        inline const std::string& GetPluginRoot()
        {
            return m_pluginRoot;
        }

        // The value GAMELIFT_ROOT where instance templates and settings are going to be stored
        inline void SetGameLiftRoot(const std::string& gameliftRoot)
        {
            m_gameliftRoot = gameliftRoot;
            m_instanceLayersPath = gameliftRoot + "/" + m_accountInfo.gameName + "/" + m_accountInfo.environment.GetEnvironmentString() + ResourceDirectories::LAYERS_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
            m_instanceFunctionsPath = gameliftRoot + "/" + m_accountInfo.gameName + "/" + m_accountInfo.environment.GetEnvironmentString() + ResourceDirectories::FUNCTIONS_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
            m_instanceCloudformationPath = gameliftRoot + "/" + m_accountInfo.gameName + "/" + m_accountInfo.environment.GetEnvironmentString() + ResourceDirectories::CLOUDFORMATION_DIRECTORY + GetFeatureTypeString(m_featureType) + "/";
        }

        // Returns the Path to a client config file where results of deployment are stored
        std::string getClientConfigFilePath() const;

        // Returns the GAMELIFT_ROOT where instance templates and settings are stored
        inline const std::string& GetGameLiftRoot()
        {
            return m_gameliftRoot;
        }

        // Sets the base CloudFormation path
        inline void SetBaseCloudFormationPath(const std::string& cloudFormationPath)
        {
            m_baseCloudformationPath = cloudFormationPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Sets the base Lambda Layers path
        inline void SetBaseLayersPath(const std::string& layersPath)
        {
            m_baseLayersPath = layersPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Sets the base Lambda Functions path
        inline void SetBaseFunctionsPath(const std::string& functionsPath)
        {
            m_baseFunctionsPath = functionsPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Sets the instance CloudFormation path
        inline void SetInstanceCloudFormationPath(const std::string& cloudFormationPath)
        {
            m_instanceCloudformationPath = cloudFormationPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Sets the instance Lambda Layers path
        void SetInstanceLayersPath(const std::string& layersPath)
        {
            m_instanceLayersPath = layersPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Sets the instance Lambda Functions path
        void SetInstanceFunctionsPath(const std::string& functionsPath)
        {
            m_instanceFunctionsPath = functionsPath + GetFeatureTypeString(m_featureType) + "/";
        }

        // Returns the base Lambda Functions path
        inline const std::string& GetBaseFunctionsPath() const
        {
            return m_baseFunctionsPath;
        }

        // Returns the base CloudFormation path
        inline const std::string& GetBaseCloudFormationPath() const
        {
            return m_baseCloudformationPath;
        }

        // Returns the instance Lambda Functions path
        inline const std::string& GetInstanceFunctionsPath() const
        {
            return m_instanceFunctionsPath;
        }

        // Returns the instance CloudFormation path
        inline const std::string& GetInstanceCloudFormationPath() const
        {
            return m_instanceCloudformationPath;
        }

        bool IsCloudFormationInstanceTemplatePresent() const;

        unsigned int SaveDeployedCloudFormationTemplate() const;
        unsigned int SaveCloudFormationInstance(const Aws::Map<std::string, std::string>& vars);
        unsigned int UpdateCloudFormationParameters(const Aws::Map<std::string, std::string>& vars);
        unsigned int SaveLayerInstances() const;
        unsigned int SaveFunctionInstances() const;

        std::string GetStackName() const;
        void SetLayersReplacementId(const std::string& replacementId);
        void SetFunctionsReplacementId(const std::string& replacementId);
        const std::string& GetFunctionsReplacementId();
        unsigned int CreateAndSetLayersReplacementId();
        unsigned int CreateAndSetFunctionsReplacementId();
        unsigned int RetrieveFunctionsReplacementId();
        unsigned int UploadDashboard(const std::string& path);
        unsigned int CompressFeatureLayers();
        unsigned int UploadFeatureLayers();
        unsigned int CompressFeatureFunctions();
        unsigned int UploadFeatureFunctions();
        std::pair<unsigned int, std::string> UpdateFeatureGameServerHash(const std::string& gameServerPath, const std::string& serverAddonsPath, std::function<unsigned int(const std::string& NewHash, const std::string& PrevHash)> onHashUpdated);
        std::pair<unsigned int, std::string> CompressFeatureGameServer(const std::string& gameServerPath, const std::string& additionalFilesPath, const std::string& gameServerHash);
        std::pair<unsigned int, std::string> UploadFeatureGameServer(const std::string& gameServerPathZipPath);
        unsigned int RemoveObjectFromBucket(const std::string& objectPath);
        void CleanupTempFiles();
        std::string GetCurrentStackStatus() const;
        unsigned int DescribeStackResources(FuncResourceInfoCallback resourceInfoCb) const;
        unsigned int CreateOrUpdateFeatureStack();
        unsigned int CancelUpdateFeatureStack();
        unsigned int WriteClientConfiguration() const;
        unsigned int DeleteFeatureStack();

        inline std::string GetLambdaFunctionReplacementIDParamName() const
        {
            return std::string(GAMELIFT_LAMBDA_FUNCTIONS_REPLACEMENT_ID_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetLambdaLayerReplacementIDParamName() const
        {
            return std::string(GAMELIFT_LAMBDA_LAYERS_REPLACEMENT_ID_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetLambdaLayerARNParamName(const std::string& layerName) const
        {
            return std::string(GAMELIFT_LAMBDA_LAYER_ARN_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(layerName)
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetLambdaLayerHashParamName(const std::string& layerName) const
        {
            return std::string(GAMELIFT_LAMBDA_LAYER_HASH_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(layerName)
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetGameServerHashParamName(const std::string& gameServerName) const
        {
            const std::string sanitizedGameServerName = createSanitizedResourceName(gameServerName);
            return std::string(GAMELIFT_GAME_SERVER_HASH_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(sanitizedGameServerName)
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetGameServerAddonsHashParamName(const std::string& gameServerName) const
        {
            const std::string sanitizedGameServerName = createSanitizedResourceName(gameServerName);
            return std::string(GAMELIFT_GAME_SERVER_ADDONS_PREFIX)
                .append(GetFeatureTypeString(m_featureType))
                .append("_")
                .append(sanitizedGameServerName)
                .append("_")
                .append(m_accountInfo.gameName)
                .append("_")
                .append(m_accountInfo.environment.GetEnvironmentString());
        }

        inline std::string GetFeatureFunctionsBucketObjectPath(const std::string& filename, const std::string& extension)
        {
            return std::string(m_accountInfo.gameName)
                .append("/functions/")
                .append(GameLift::GetFeatureTypeString(m_featureType))
                .append("/")
                .append(filename)
                .append(".")
                .append(m_functionsReplacementId)
                .append(extension);
        }

        inline std::string GetDashboardBucketObjectPath()
        {
            return std::string(m_accountInfo.gameName)
                .append("/cloudformation/")
                .append(GameLift::GetFeatureTypeString(m_featureType))
                .append("/")
                .append(TemplateFileNames::FEATURE_DASHBOARD_FILE);
        }

        inline std::string GetFeatureLayersBucketObjectPath(const std::string& filename, const std::string& extension)
        {
            return std::string(m_accountInfo.gameName)
                .append("/layers/")
                .append(GameLift::GetFeatureTypeString(m_featureType))
                .append("/")
                .append(filename)
                .append(".")
                .append(m_layersReplacementId)
                .append(extension);
        }

        inline void SetS3Client(Aws::S3::S3Client* s3Client, bool isShared)
        {
            m_isUsingSharedS3Client = isShared;
            m_s3Client = s3Client;
        }

        inline void SetSSMClient(Aws::SSM::SSMClient* ssmClient, bool isShared)
        {
            m_isUsingSharedSSMClient = isShared;
            m_ssmClient = ssmClient;
        }

        inline void SetCloudFormationClient(Aws::CloudFormation::CloudFormationClient* cfClient, bool isShared)
        {
            m_isUsingSharedCfClient = isShared;
            m_cfClient = cfClient;
        }

        inline void SetLambdaClient(Aws::Lambda::LambdaClient* lambdaClient, bool isShared)
        {
            m_isUsingSharedLambdaClient = isShared;
            m_lambdaClient = lambdaClient;
        }

        const std::string GetUnrealPackagedBuildVersion(const std::string& executablePath) const;

        static std::string GetGameServerBucketPath(const std::string& gameName, const std::string& gameServerPath);
        static std::string GetGameServerZipName(const std::string& gameServerPath, const std::string& gameServerHash);
    };
}
