// Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.

/** @file
 * @brief The C interface for the Core library.
 */

#pragma once

// Standard Library
#include <exception>
#include <string>

// GameLift
#include <aws/gamelift/core/api.h>
#include <aws/gamelift/core/errors.h>
#include <aws/gamelift/core/feature_resources.h>
#include <aws/gamelift/core/gamelift_account.h>
#include <aws/gamelift/core/gamelift_settings.h>
#include <aws/gamelift/core/logging.h>
#include <aws/gamelift/core/awsclients/default_clients.h>
#include <aws/gamelift/core/model/account_credentials.h>
#include <aws/gamelift/core/model/account_info.h>
#include <aws/gamelift/core/utils/sts_utils.h>
#include <aws/gamelift/model/GameSessionStatus.h>
#include <aws/gamelift/model/GameSessionStatusReason.h>
#include <aws/gamelift/model/PlayerSessionStatus.h>
#include <aws/gamelift/GameLiftErrors.h>

#define MAX_ANYWHERE_FLEETS 20
#define MAX_ANYWHERE_STR_LEN 1024
#define MAX_REPO_COUNT 20
#define MAX_REPO_STR_LEN 1024
#define MAX_IMAGE_COUNT 20
#define MAX_IMAGE_STR_LEN 1024
#define MAX_CGD_COUNT 20
#define MAX_CGD_STR_LEN 1024
#define MAX_CGD_STATUS_STR_LEN 32
#define MAX_FLEETID_STR_LEN 64

/**
 * @brief  GameLiftAccount instance handle created by calling #GameLiftAccountInstanceCreate()
*/
typedef void* GAMELIFT_ACCOUNT_INSTANCE_HANDLE;

/**
 * @brief GameLiftFeatureResources instance handle created by calling #GameLiftResourcesInstanceCreate()
*/
typedef void* GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE;

/**
 * @brief GameLiftSettings instance handle created by calling #GameLiftSettingsInstanceCreate()
*/
typedef void* GAMELIFT_SETTINGS_INSTANCE_HANDLE;

/**
 * @brief A pointer to an instance of a class that can receive a callback.
 *
 * @details The callback method signature is specified by each API which uses a DISPATCH_RECEIVER_HANDLE.
 *
 * For example: GameLiftSettingsGetGameName() uses a callback signature of CharPtrCallback,
 * whereas GameLiftSettingsGetCustomEnvironments() uses a callback signature of KeyValueCharPtrCallbackDispatcher.
 */
typedef void* DISPATCH_RECEIVER_HANDLE;

/**
 * @brief GameLiftClient instance handle created by calling #GameLiftClientInstanceCreate()
*/
typedef void* GAMELIFT_CLIENT_INSTANCE_HANDLE;

/**
 * @brief GameLift game session instance handle created by calling #GameLiftGameSessionCreate()
*/
typedef void* GAMELIFT_GAMESESSION_INSTANCE_HANDLE;

/**
 * @brief GameLift player session instance handle created by calling #GameLiftPlayerSessionCreate()
*/
typedef void* GAMELIFT_PLAYERSESSION_INSTANCE_HANDLE;

extern "C"
{
    /**
     * @brief A static dispatcher function pointer that receives a character array.
     *
     * @param dispatchReceiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param charPtr The character array pointer that the callback function receives.
     */
    typedef void(*CharPtrCallback)(DISPATCH_RECEIVER_HANDLE dispatchReceiver, const char* charPtr);

    /**
     * @brief A static dispatcher function pointer that receives key/value pairs, one invocation at a time.
     *
     * @param dispatchReceiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have have a method signature of void ReceiveResult(const char* charKey, const char* charValue);
     * @param charKey The key that the callback function receives.
     * @param charValue The value that the callback function receives.
     */
    typedef void(*KeyValueCharPtrCallbackDispatcher)(DISPATCH_RECEIVER_HANDLE dispatchReceiver, const char* charKey, const char* charValue);

    /**
     * @brief Error info passed to a error handler callback #FuncErrorCallback when calling #GameLiftClientInstanceCreate(), #GameLiftGameSessionCreate(), #GameLiftPlayerSessionCreate(), and Anywhere APIs.
     */
    struct GAMELIFT_ERROR_INFO {
        Aws::GameLift::GameLiftErrors errorType = Aws::GameLift::GameLiftErrors::UNKNOWN;
        const char* errorMessage = nullptr;
    };

    /**
     * @brief Error handler callback. Refer to #GAMELIFT_ERROR_HANDLER.
     */
    typedef void (*FuncErrorCallback)(DISPATCH_RECEIVER_HANDLE errorReceiver, GAMELIFT_ERROR_INFO* errorInfo);

    /**
     * @brief Error handler passed as a part of request when calling #GameLiftClientInstanceCreate(), #GameLiftGameSessionCreate(), #GameLiftPlayerSessionCreate(), and Anywhere APIs.
     */
    struct GAMELIFT_ERROR_HANDLER {
        FuncErrorCallback errorCb = nullptr;
        DISPATCH_RECEIVER_HANDLE errorReceiver = nullptr;
    };
}

extern "C"
{

#pragma region GameLiftAccount
    // -------- Static functions, these don't require a GameLiftAccount instance handle
    /**
     * @brief Get the AWS Account ID which corresponds to the provided Access Key and Secret Key.
     *
     * @details For more information about AWS access keys and secret keys, see: https://docs.aws.amazon.com/general/latest/gr/aws-sec-cred-types.html#access-keys-and-secret-access-keys
     *
     * @param dispatchReceiver Pointer to the caller object (object that will handle the callback function).
     * @param resultCb Pointer to the callback function to invoke on completion.
     * @param accessKey The AWS Access Key.
     * @param secretKey The AWS Secret Key.
     * @param logCb Callback function for logging information and errors.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftGetAwsAccountId(DISPATCH_RECEIVER_HANDLE dispatchReceiver, CharPtrCallback resultCb, const char* accessKey, const char* secretKey, FuncLogCallback logCb);

    // -------- Instance functions, these require a GameLiftAccount instance handle
    /**
     * @brief Create a GameLiftAccount instance, which can be used to access the GameLiftAccount API.
     *
     * @details Make sure to call GameLiftAccountInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInfo Struct holding account id, game name, and deployment environment.
     * @param credentials Struct holding account id, region, access key, and secret key.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftAccount instance.
     */
     // Deprecated
    GAMELIFT_API GAMELIFT_ACCOUNT_INSTANCE_HANDLE GameLiftAccountInstanceCreate(GameLift::AccountInfo accountInfo, GameLift::AccountCredentials credentials, FuncLogCallback logCb);

    /**
     * @brief Create a GameLiftAccount instance, which can be used to access the GameLiftAccount API. Also sets the plugin and game root paths.
     *
     * @details Make sure to call GameLiftAccountInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInfo Struct holding account id, game name, and deployment environment.
     * @param credentials Struct holding account id, region, access key, and secret key.
     * @param rootPath New path for GAMELIFT_ROOT
     * @param pluginRootPath New path for the plugin root directory.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftAccount instance.
     */
    GAMELIFT_API GAMELIFT_ACCOUNT_INSTANCE_HANDLE GameLiftAccountInstanceCreateWithRootPaths(GameLift::AccountInfo accountInfo, GameLift::AccountCredentials credentials, const char* rootPath, const char* pluginRootPath, FuncLogCallback logCb);

    /**
     * @brief Create a GameLiftAccount instance, which can be used to access the GameLiftAccount API. Also sets the plugin and game root paths.
     *
     * @details Make sure to call GameLiftAccountInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInfo Struct holding account id, game name, and deployment environment.
     * @param profileName Profile name int the shared AWS credentials file.
     * @param rootPath New path for GAMELIFT_ROOT
     * @param pluginRootPath New path for the plugin root directory.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftAccount instance.
     */
    GAMELIFT_API GAMELIFT_ACCOUNT_INSTANCE_HANDLE GameLiftAccountInstanceCreateWithRootPathsAndProfile(GameLift::AccountInfo accountInfo, const char* profileName, const char* rootPath, const char* pluginRootPath, FuncLogCallback logCb);

    /**
     * @brief Destroy the provided GameLiftAccount instance.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     */
    GAMELIFT_API void GameLiftAccountInstanceRelease(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the AWS Account ID which corresponds to the provided Account Instance.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return AWS Account Id.
     */
    GAMELIFT_API const char* GameLiftGetAwsAccountIdByAccountInstance(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Generate the default bootstrap bucket name based on current account info.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return generated bucket name.
     */
    GAMELIFT_API const char* GameLiftAccountGetDefaultBucketName(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the existing boostrap bucket name for the current account.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return bucket name, or an empty string if GameLiftAccountInstanceBootstrap() or GameLiftAccountSetBucketName() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetBucketName(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Set the bucket name for the current account overwrites value provided by GameLiftAccountInstanceCreate().
     *
     * @details This value can be fetched by GameLiftAccountGetBucketName() and defaults to an empty string until this method is called.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param bucketName New value for Bucket Name.
     */
    GAMELIFT_API void GameLiftAccountSetBucketName(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* bucketName);

    /**
     * @brief Get the game name for the current account.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return game name, or an empty string if GameLiftAccountSetGameName() hasn't been called yet or if not specified in GameLiftAccountInstanceCreate().
     */
    GAMELIFT_API const char* GameLiftAccountGetGameName(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Set the game name for the current account overwrites value provided by GameLiftAccountInstanceCreate().
     *
     * @details This value can be fetched by GameLiftAccountGetGameName() and defaults to an empty string until this method or GameLiftAccountInstanceCreate() is called.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param gameName New value for Game Name.
     */
    GAMELIFT_API void GameLiftAccountSetGameName(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* gameName);

    /**
     * @brief Get the GAMELIFT_ROOT path where the "instance" templates and settings are going to be stored.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return GAMELIFT_ROOT path, or an empty string if GameLiftAccountSetRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetRootPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Set the GAMELIFT_ROOT path where the "instance" templates and settings are going to be stored.
     *
     * @details This value can be fetched by GameLiftAccountGetRootPath() and defaults to an empty string until this method is called.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param rootPath New path for GAMELIFT_ROOT.
     */
    GAMELIFT_API void GameLiftAccountSetRootPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* rootPath);

    /**
     * @brief Get the root directory of the GAMELIFT plugin's installation.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Plugin root path, or an empty string if GameLiftAccountSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetPluginRootPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Set the root directory where the GAMELIFT plugin was installed.
     *
     * @details This value can be fetched by GameLiftAccountGetPluginRootPath() and defaults to an empty string until this method is called.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param pluginRootPath New path for the plugin root directory.
     */
    GAMELIFT_API void GameLiftAccountSetPluginRootPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* pluginRootPath);

    /**
     * @brief Get the path where the "base" CloudFormation templates are stored.
     *
     * @details This path is set by calling GameLiftAccountSetPluginRootPath(), and is equal to GameLiftAccountGetPluginRootPath()+ResourceDirectories::CLOUDFORMATION_DIRECTORY.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Path to the "base" CloudFormation templates, or an empty string if GameLiftAccountSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetBaseCloudFormationPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the path where the "base" Lambda functions are stored.
     *
     * @details This path is set by calling GameLiftAccountSetPluginRootPath(), and is equal to GameLiftAccountGetPluginRootPath()+ResourceDirectories::FUNCTIONS_DIRECTORY.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Path to the "base" Lambda functions, or an empty string if GameLiftAccountSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetBaseFunctionsPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the path where the "instance" CloudFormation templates are stored.
     *
     * @details This path is set by calling GameLiftAccountSetRootPath(), and is equal to GameLiftAccountGetRootPath()+<gameName>+ResourceDirectories::CLOUDFORMATION_DIRECTORY;.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Path to the "instance" CloudFormation templates, or an empty string if GameLiftAccountSetRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetInstanceCloudFormationPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the path where the "instance" Lambda functions are stored.
     *
     * @details This path is set by calling GameLiftAccountSetRootPath(), and is equal to GameLiftAccountGetRootPath()+<gameName>+ResourceDirectories::FUNCTIONS_DIRECTORY.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Path to the "instance" Lambda functions, or an empty string if GameLiftAccountSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftAccountGetInstanceFunctionsPath(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Return True if the provided GameLiftAccount instance has valid AWS credentials (access key, secret key, and AWS region), return False otherwise.
     *
     * @details In this case, "valid" means the credentials are allowed to list S3 buckets (i.e. the IAM Role has "s3:ListAllMyBuckets" permission).
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return True if the credentials are valid, false otherwise.
     */
    GAMELIFT_API bool GameLiftAccountHasValidCredentials(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Create a bootstrap bucket in the AWS account if it doesn't already exist.
     *
     * @details The bootstrap bucket must be created before deploying any stacks or Lambda functions.
     * There needs to be a unique bootstrap bucket for each combination of Environment, Region, Account ID, and GameName.
     *
     * @details The bucket name will be "do-not-delete-gamelift-<env>-<5_letter_aws_region_code>-<base36_account_id>-<gamename>"
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountInstanceBootstrap(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, DISPATCH_RECEIVER_HANDLE dispatchReceiver = nullptr, CharPtrCallback resultCb = nullptr);

    /**
     * @brief Create or update a secret in AWS SecretsManager (https://aws.amazon.com/secrets-manager/).
     *
     * @details The secret name will be "gamelift_<environment>_<gameName>_<secretName>", for example: "gamelift_dev_mygame_amazon_client_secret".
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param secretName Name of the secret. Will be prefixed as described in the details.
     * @param secretValue Value of the secret.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountSaveSecret(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* secretName, const char* secretValue);

    /**
     * @brief Checks if a secret exists in AWS SecretsManager (https://aws.amazon.com/secrets-manager/).
     *
     * @details The secret name will be "gamelift_<environment>_<gameName>_<secretName>", for example: "gamelift_dev_mygame_amazon_client_secret".
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param secretName Name of the secret. Will be prefixed as described in the details.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountCheckSecretExists(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* secretName);

    /**
     * @brief Delete a secret in AWS SecretsManager (https://aws.amazon.com/secrets-manager/).
     *
     * @details The secret name will be "gamelift_<environment>_<gameName>_<secretName>", for example: "gamelift_dev_mygame_amazon_client_secret".
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param secretName Name of the secret. Will be prefixed as described in the details.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful or non-existent, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountDeleteSecret(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* secretName);

    /**
     * @brief Create or overwrite the instance template files for every feature.
     *
     * @details The parameters "varKeys", "varValues", and "numKeys" represent a map<string, string>, where varKeys[N] maps to varValues[N], and numKeys is the total number of key-value pairs.
     * The key-value pairs are used to replace placeholder variables prefixed with "AWSGAMELIFT::VARS::" in the CloudFormation templates and CloudFormation parameters files.
     *
     * @details Creates the following instance files for each feature: CloudFormation template, CloudFormation parameters, and Lambda functions.
     * Instance files are written to the "GAMELIFT_ROOT" path, and are created as copies of the "Base" path files with the placeholder variables filled in.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param varKeys Keys of the placeholders to replace. For example, "my_key" in the following placeholder: "AWSGAMELIFT::VARS::my_key".
     * @param varValues Values to replace the placeholders with.
     * @param numKeys The number of key-value pairs. The length of varKeys, varValues, and numKeys should be equal.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountSaveFeatureInstanceTemplates(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* const* varKeys, const char* const* varValues, size_t numKeys);

    /**
     * @brief Upload the Dashboard configuration file for every feature to the bootstrap bucket.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountUploadAllDashboards(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Upload a game server to the bootstrap bucket. Returns a path of a resulted S3 bucket file into resultsCb synchronously.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param serverPath A path to a local game server build folder.
     * @param addPath A path to the folder with additional content for server.
     * @param dispatchReceiver A pointer to an instance of a class where the results will be dispatched to. This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param resultsCb A static dispatcher function pointer that receives key/value pairs.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountUploadGameServerEx(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* serverPath, const char* addPath, DISPATCH_RECEIVER_HANDLE dispatchReceiver, CharPtrCallback resultsCb);

    /**
     * @brief Upload a game server to the bootstrap bucket.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param serverPath A path to a local game server build folder.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountUploadGameServer(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, const char* serverPath);

    /**
     * @brief Upload the Lambda instance layers for every feature to the bootstrap bucket.
     *
     * @details GameLiftAccountSaveFeatureInstanceTemplates() should be called first to create the Lambda instance templates.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountUploadLayers(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Upload the Lambda instance functions for every feature to the bootstrap bucket.
     *
     * @details GameLiftAccountSaveFeatureInstanceTemplates() should be called first to create the Lambda instance templates.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountUploadFunctions(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Deploy the "main" CloudFormation stack to AWS.
     *
     * @details Should call GameLiftAccountSaveFeatureInstanceTemplates() first to create the instance templates.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountCreateOrUpdateMainStack(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Create unique ids for all function folders under the plugin root and store them as parameters to AWS.
     *
     * @details Should be called before GameLiftAccountGetMainFunctionsReplacementId() is invoked.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountCreateAndSetFunctionsReplacementId(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Get the functions replacement id for the "main" functions.
     *
     * @details Should call GameLiftAccountCreateAndSetFunctionsReplacementId() first to create functions replacement id.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @param dispatchReceiver A pointer to an instance of a class where the results will be dispatched to. This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param charPtr The character array pointer that the callback function receives.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountGetMainFunctionsReplacementId(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, DISPATCH_RECEIVER_HANDLE dispatchReceiver, CharPtrCallback resultCb);

    /**
     * @brief Deploy all CloudFormation stacks to AWS (i.e. the "main" stack and all feature stacks).
     *
     * @details Should call GameLiftAccountSaveFeatureInstanceTemplates() first to create the instance templates.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountCreateOrUpdateStacks(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Deploy the API Gateway stage of the "main" CloudFormation stack.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftAccountDeployApiGatewayStage(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);

    /**
     * @brief Create a GameLiftFeatureResources instance, which can be used to access the GameLiftFeatureResources API. Also sets the root and pluginRoot paths.
     *
     * @details Make sure to call GameLiftResourcesInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance created with GameLiftAccountInstanceCreate().
     * @return Pointer to the new GameLiftFeatureResources instance.
     */
    GAMELIFT_API GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE GameLiftAccountCreateGameLiftResourcesInstance(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance);
#pragma endregion

#pragma region GameLiftFeatureResources
    /**
     * @brief Create a GameLiftFeatureResources instance, which can be used to access the GameLiftFeatureResources API.
     *
     * @details Make sure to call GameLiftResourcesInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInfo Struct holding account id, game name, and deployment environment.
     * @param credentials Struct holding account id, region, access key, and secret key.
     * @param featureType The GAMELIFT feature to work with.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftFeatureResources instance.
     */
    // Deprecated
    GAMELIFT_API GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE GameLiftResourcesInstanceCreate(GameLift::AccountInfo accountInfo, GameLift::AccountCredentials credentials, GameLift::FeatureType featureType, FuncLogCallback logCb);

    /**
     * @brief Create a GameLiftFeatureResources instance, which can be used to access the GameLiftFeatureResources API. Also sets the root and pluginRoot paths.
     *
     * @details Make sure to call GameLiftResourcesInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param accountInfo Struct holding account id, game name, and deployment environment.
     * @param credentials Struct holding account id, region, access key, and secret key.
     * @param featureType The GAMELIFT feature to work with.
     * @param rootPath New path for GAMELIFT_ROOT.
     * @param pluginRootPath New path for the plugin root directory.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftFeatureResources instance.
     */
    GAMELIFT_API GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE GameLiftResourcesInstanceCreateWithRootPaths(GameLift::AccountInfo accountInfo, GameLift::AccountCredentials credentials, GameLift::FeatureType featureType, const char* rootPath, const char* pluginRootPath, FuncLogCallback logCb);

    /**
     * @brief Destroy the provided GameLiftFeatureResources instance.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     */
    GAMELIFT_API void GameLiftResourcesInstanceRelease(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the GAMELIFT_ROOT path where the "instance" templates and settings are going to be stored.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return GAMELIFT_ROOT path, or an empty string if GameLiftResourcesSetRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetRootPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Set the GAMELIFT_ROOT path where the "instance" templates and settings are going to be stored.
     *
     * @details This value can be fetched by GameLiftResourcesGetRootPath() and defaults to an empty string until this method is called.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param rootPath New path for GAMELIFT_ROOT.
     */
    GAMELIFT_API void GameLiftResourcesSetRootPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, const char* rootPath);

    /**
     * @brief Get the root directory of the GAMELIFT plugin's installation.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return Plugin root path, or an empty string if GameLiftResourcesSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetPluginRootPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Set the root directory where the GAMELIFT plugin was installed.
     *
     * @details This value can be fetched by GameLiftResourcesGetPluginRootPath() and defaults to an empty string until this method is called.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param pluginRootPath New path for the plugin root directory.
     */
    GAMELIFT_API void GameLiftResourcesSetPluginRootPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, const char* pluginRootPath);

    /**
     * @brief Get the path where this feature's "base" CloudFormation template is stored.
     *
     * @details This path is set by calling GameLiftResourcesSetPluginRootPath(), and is equal to GameLiftResourcesGetPluginRootPath()+ResourceDirectories::CLOUDFORMATION_DIRECTORY+<feature_name>.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return Path to this feature's "base" CloudFormation template, or an empty string if GameLiftResourcesSetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetBaseCloudFormationPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the path where this feature's "base" Lambda functions are stored.
     *
     * @details This path is set by calling GameLiftResourcesSetPluginRootPath(), and is equal to GameLiftResourcesGetPluginRootPath()+ResourceDirectories::FUNCTIONS_DIRECTORY+<feature_name>.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return Path to this feature's "base" Lambda functions, or an empty string if GameLiftResourcesGetPluginRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetBaseFunctionsPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the path where this feature's "instance" CloudFormation template is stored.
     *
     * @details This path is set by calling GameLiftResourcesSetRootPath(), and is equal to GameLiftResourcesGetRootPath()+<gameName>+ResourceDirectories::CLOUDFORMATION_DIRECTORY+<feature_name>;.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return Path to this feature's "instance" CloudFormation template, or an empty string if GameLiftResourcesGetRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetInstanceCloudFormationPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the path where this feature's "instance" Lambda functions are stored.
     *
     * @details This path is set by calling GameLiftResourcesSetRootPath(), and is equal to GameLiftResourcesGetRootPath()+<gameName>+ResourceDirectories::FUNCTIONS_DIRECTORY+<feature_name>.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return Path to this feature's "instance" Lambda functions, or an empty string if GameLiftResourcesGetRootPath() hasn't been called yet.
     */
    GAMELIFT_API const char* GameLiftResourcesGetInstanceFunctionsPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the path where this feature's "instance" client config file is stored.
     *
     * @details This path is set by calling GameLiftResourcesSetRootPath(), and is equal to GameLiftResourcesGetRootPath()+<gameName>+<env>+<TemplateFileNames::GAMELIFT_CLIENT_CONFIGURATION_FILE>.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param dispatchReceiver A pointer to an instance of a class where the results will be dispatched to. This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param charPtr The character array pointer that the callback function receives.
     * @return Path to this feature's "instance" client config file, or an empty string if GameLiftResourcesSetRootPath() hasn't been called yet.
     */
    GAMELIFT_API void GameLiftResourcesGetInstanceClientConfigPath(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, DISPATCH_RECEIVER_HANDLE dispatchReceiver, CharPtrCallback resultsCb);


    /**
     * @brief Deploy this feature's CloudFormation stack to AWS.
     *
     * @details Deploys the instance CloudFormation template. Creates a new stack if no stack exists, or updates an existing stack.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesInstanceCreateOrUpdateStack(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Delete this feature's CloudFormation stack from AWS.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesInstanceDeleteStack(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Cancel deploying current feature's CloudFormation stack to AWS.
     *
     * @details Cancel deploying of the current instance CloudFormation template.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesInstanceCancelUpdateStack(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the status of this feature's deployed CloudFormation stack, such as "CREATE_COMPLETE" or "UPDATE_IN_PROGRESS".
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The stack status string, or "UNDEPLOYED" if the stack is not deployed.
     */
    GAMELIFT_API const char* GameLiftResourcesGetCurrentStackStatus(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Checks if feature's CloudFormation template is present in the feature's instance directory.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return true if the feature's CloudFormation template is present in the feature's instance directory.
     */
    GAMELIFT_API bool GameLiftResourcesIsCloudFormationInstanceTemplatePresent(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Retrieves the feature's deployed CloudFormation template and saves it to the feature's instance directory.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesSaveDeployedCloudFormationTemplate(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Create or overwrite this feature's instance CloudFormation template file and CloudFormation parameters file.
     *
     * @details The parameters "varKeys", "varValues", and "numKeys" represent a map<string, string>, where varKeys[N] maps to varValues[N], and numKeys is the total number of key-value pairs.
     * The key-value pairs are used to replace placeholder variables prefixed with "AWSGAMELIFT::VARS::" in the CloudFormation templates and CloudFormation parameters files.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param varKeys Keys of the placeholders to replace. For example, "my_key" in the following placeholder: "AWSGAMELIFT::VARS::my_key".
     * @param varValues Values to replace the placeholders with.
     * @param numKeys The number of key-value pairs. The length of varKeys, varValues, and numKeys should be equal.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesSaveCloudFormationInstance(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, const char* const* varKeys, const char* const* varValues, size_t numKeys);

    /**
     * @brief Update this feature's instance CloudFormation parameters file.
     *
     * @details The parameters "varKeys", "varValues", and "numKeys" represent a map<string, string>, where varKeys[N] maps to varValues[N], and numKeys is the total number of key-value pairs.
     * The key-value pairs are used to replace placeholder variables prefixed with "AWSGAMELIFT::VARS::" in the CloudFormation parameters files.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param varKeys Keys of the placeholders to replace. For example, "my_key" in the following placeholder: "AWSGAMELIFT::VARS::my_key".
     * @param varValues Values to replace the placeholders with.
     * @param numKeys The number of key-value pairs. The length of varKeys, varValues, and numKeys should be equal.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesUpdateCloudFormationParameters(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, const char* const* varKeys, const char* const* varValues, size_t numKeys);

    /**
     * @brief Create or overwrite this feature's instance Lambda layer files.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesSaveLayerInstances(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Create or overwrite this feature's instance Lambda function files.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesSaveFunctionInstances(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Get the file version of the packaged build executable.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The string containing the file version information of executable.
     */
    GAMELIFT_API const char* GameLiftResourcesGetUnrealPackagedBuildVersion(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, const std::string& executablePath);

    /**
     * @brief Upload this feature's instance Lambda layers to the S3 bootstrap bucket.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesUploadFeatureLayers(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Upload this feature's instance Lambda functions to the S3 bootstrap bucket.
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesUploadFeatureFunctions(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance);

    /**
     * @brief Log the current status of all the resources in this feature's CloudFormation stack.
     *
     * @details For example, a resource's status may look like: "CognitoLambdaRole, AWS::IAM::Role, CREATE_COMPLETE".
     *
     * @param resourceInstance Pointer to a GameLiftFeatureResources instance created with GameLiftResourcesInstanceCreate().
     * @param resourceInfoCb Callback function for logging the resources statuses.
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftResourcesDescribeStackResources(GAMELIFT_FEATURERESOURCES_INSTANCE_HANDLE resourceInstance, FuncResourceInfoCallback resourceInfoCb);
#pragma endregion

#pragma region GameLiftSettings
    /**
     * @brief Create a GameLiftSettings instance and load the settings from the GAMELIFT Settings YAML file.
     *
     * @details Make sure to call GameLiftSettingsInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param rootPath The GAMELIFT_ROOT path where the "instance" templates and settings are stored.
     * @param pluginVersion The GAMELIFT plugin version.
     * @param currentEnvironment The current active environment eg "dev", "qa", custom
     * @param shortGameName A shortened version of the game name.
     * @param logCb Callback function for logging information and errors.
     * @return Pointer to the new GameLiftSettings instance.
     */
    GAMELIFT_API GAMELIFT_SETTINGS_INSTANCE_HANDLE GameLiftSettingsInstanceCreate(const char* rootPath, const char* pluginVersion, const char* shortGameName, const char* currentEnvironment, FuncLogCallback logCb);

    /**
     * @brief Destroy the provided GameLiftSettings instance.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     */
    GAMELIFT_API void GameLiftSettingsInstanceRelease(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance);

    /**
     * @brief Set the game's name.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param gameName The new name.
     */
    GAMELIFT_API void GameLiftSettingsSetGameName(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, const char* gameName);

    /**
     * @brief Set the last used region.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param region The region.
     */
    GAMELIFT_API void GameLiftSettingsSetLastUsedRegion(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, const char* region);

    /**
     * @brief Set the last used environment.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param gameName The environment code.
     */
    GAMELIFT_API void GameLiftSettingsSetLastUsedEnvironment(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, const char* envCode);

    /**
     * @brief Add a custom deployment environment to the AWS Control Center menu.
     *
     * @details This custom environment will be available to select from the dropdown menu in the plugin's AWS Control Center,
     * alongside the default environments of "Development", "QA", "Staging", and "Production".
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param envCode Three letter code for the environment name. This code will be prefixed on all AWS resources that are
     * deployed to this environment. Ex: "gam" for "Gamma".
     * @param envDescription The environment name that will be displayed in the AWS Control Center. Ex: "Gamma".
     */
    GAMELIFT_API void GameLiftSettingsAddCustomEnvironment(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, const char* envCode, const char* envDescription);

    /**
     * @brief Remove a custom deployment environment from the AWS Control Center menu.
     *
     * @details Note: If you intend to delete the stacks deployed to this environment, you should delete them first
     * before deleting the custom environment. Otherwise you'll have to manually delete them from the AWS CloudFormation webpage.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param envCode Three letter code for the custom environment to delete. Ex: "gam" for "Gamma".
     */
    GAMELIFT_API void GameLiftSettingsDeleteCustomEnvironment(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, const char* envCode);

    /**
     * @brief Activate a GAMELIFT feature (ex: "Achievements").
     *
     * @details After activating, the feature will be available to configure and deploy.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param featureType The feature to activate.
     */
    GAMELIFT_API void GameLiftSettingsActivateFeature(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, GameLift::FeatureType featureType);

    /**
     * @brief Deactivate a GAMELIFT feature (ex: "Achievements").
     *
     * @details After deactivating, the feature will not be able to be configured or deployed.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param featureType The feature to deactivate.
     */
    GAMELIFT_API void GameLiftSettingsDeactivateFeature(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, GameLift::FeatureType featureType);

    /**
     * @brief Add key-value pairs to the feature's variables map, overwriting existing keys.
     *
     * @details The parameters "varKeys", "varValues", and "numKeys" represent a map<string, string>, where varKeys[N] maps to varValues[N], and numKeys is the total number of key-value pairs.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param featureType The feature to set the variables for.
     * @param varKeys The variable names.
     * @param varValues The variable values.
     * @param numKeys The number of key-value pairs. The length of varKeys, varValues, and numKeys should be equal.
     */
    GAMELIFT_API void GameLiftSettingsSetFeatureVariables(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, GameLift::FeatureType featureType, const char* const* varKeys, const char* const* varValues, size_t numKeys);

    /**
     * @brief Delete a key-value pair from the feature's variables map.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param featureType The feature to delete the variable from.
     * @param varName The variable name to delete.
     */
    GAMELIFT_API void GameLiftSettingsDeleteFeatureVariable(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, GameLift::FeatureType featureType, const char* varName);

    /**
     * @brief Write the GAMELIFT Settings YAML file to disk.
     *
     * @details Call this to persist any changes made through the "Set", "Add/Delete", "Activate/Deactivate" methods.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @return The result code of the operation. GAMELIFT_SUCCESS if successful, else a non-zero value in case of error. Consult errors.h file for details.
     */
    GAMELIFT_API unsigned int GameLiftSettingsSave(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance);

    /**
     * @brief Get the game's full name, example: "My Full Game Name".
     *
     * @details The game name is returned through the callback and receiver.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param resultsCb A static dispatcher function pointer that receives a character array.
     */
    GAMELIFT_API void GameLiftSettingsGetGameName(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, CharPtrCallback resultsCb);

    /**
     * @brief Get the developer's last submitted region, example: "us-west-2".
     *
     * @details The region is returned through the callback and receiver. If no last used region exists, defaults to us-east-1.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param resultsCb A static dispatcher function pointer that receives a character array.
     */
    GAMELIFT_API void GameLiftSettingsGetLastUsedRegion(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, CharPtrCallback resultsCb);

    /**
   * @brief Get the developers last submitted environment code, example: "dev".
   *
   * @details The environment code is returned through the callback and receiver. If no last used environment exists, defaults to the dev environment.
   *
   * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
   * @param receiver A pointer to an instance of a class where the results will be dispatched to.
   * This instance must have a method signature of void ReceiveResult(const char* charPtr);
   * @param resultsCb A static dispatcher function pointer that receives a character array.
   */
    GAMELIFT_API void GameLiftSettingsGetLastUsedEnvironment(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, CharPtrCallback resultsCb);


    /**
     * @brief Get all the custom environment key-value pairs (ex: "gam", "Gamma").
     *
     * @details The custom environments are returned through the callback and receiver.
     * The callback is invoked once for each custom environment.
     * The returned keys are 3-letter environment codes (ex: "gam"), and the values are corresponding environment descriptions (ex: "Gamma").
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have have a method signature of void ReceiveResult(const char* charKey, const char* charValue);
     * @param resultsCb A static dispatcher function pointer that receives key/value pairs.
     */
    GAMELIFT_API void GameLiftSettingsGetCustomEnvironments(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, KeyValueCharPtrCallbackDispatcher resultsCb);

    /**
     * @brief Get the custom environment description (ex: "Gamma") for the provided environment code (ex: "gam").
     *
     * @details The environment description is returned through the callback and receiver.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param envCode The 3-letter environment code to get the description for.
     * @param resultsCb A static dispatcher function pointer that receives a character array.
     */
    GAMELIFT_API void GameLiftSettingsGetCustomEnvironmentDescription(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, const char* envCode, CharPtrCallback resultsCb);

    /**
     * @brief Return True if the feature is active, return false if not active.
     *
     * @details A feature can be activated/deactivated with the functions: GameLiftSettingsActivateFeature() and GameLiftSettingsDeactivateFeature()
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param featureType The feature to check.
     * @return True if the feature is active, false if not active.
     */
    GAMELIFT_API bool GameLiftSettingsIsFeatureActive(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, GameLift::FeatureType featureType);

    /**
     * @brief Get all of the feature's variables as key-value pairs.
     *
     * @details The variables are returned through the callback and receiver.
     * The callback is invoked once for each variable. The variables are returned as key-value pairs of (variableName, variableValue).
     * The callback will not be invoked if the feature is missing from the settings file.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have have a method signature of void ReceiveResult(const char* charKey, const char* charValue);
     * @param featureType The feature to get the variables for.
     * @param resultsCb A static dispatcher function pointer that receives key/value pairs.
     */
    GAMELIFT_API void GameLiftSettingsGetFeatureVariables(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, GameLift::FeatureType featureType, KeyValueCharPtrCallbackDispatcher resultsCb);

    /**
     * @brief Get the value of the specified feature's variable.
     *
     * @details The value is returned through the callback and receiver.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param featureType The feature to get the variable for.
     * @param varName The name of the variable to get the value for.
     * @param resultsCb A static dispatcher function pointer that receives a character array.
     */
    GAMELIFT_API void GameLiftSettingsGetFeatureVariable(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, GameLift::FeatureType featureType, const char* varName, CharPtrCallback resultsCb);

    /**
     * @brief Get the path to the "saveInfo.yml" settings file.
     *
     * @details The path is equal to "GAMELIFT_ROOT/shortGameName/saveInfo.yml".
     * The path is returned through the callback and receiver.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     * @param receiver A pointer to an instance of a class where the results will be dispatched to.
     * This instance must have a method signature of void ReceiveResult(const char* charPtr);
     * @param resultsCb A static dispatcher function pointer that receives a character array.
     */
    GAMELIFT_API void GameLiftSettingsGetSettingsFilePath(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance, DISPATCH_RECEIVER_HANDLE receiver, CharPtrCallback resultsCb);

    /**
     * @brief Reload the GAMELIFT Settings YAML file from disk.
     *
     * @details Overwrites any changes made through the "Set", "Add/Delete", "Activate/Deactivate" methods.
     *
     * @param settingsInstance Pointer to a GameLiftSettings instance created with GameLiftSettingsInstanceCreate().
     */
    GAMELIFT_API void GameLiftSettingsReload(GAMELIFT_SETTINGS_INSTANCE_HANDLE settingsInstance);
#pragma endregion

#pragma region GameLiftClient

    /**
     * @brief Create Anywhere custom location request structure, provided to #GameLiftAnywhereCreateCustomLocation().
     *
     * @details If succeedIfExisting is set to true, the operation would succeed even if the location was already created.
     */
    struct GAMELIFT_ANYWHERE_CREATE_LOCATION_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* customLocation = nullptr;
        bool succeedIfExisting = false;
    };

    /**
     * @brief Add Anywhere custom location to fleet request structure, provided to #GameLiftAnywhereAddFleetLocation().
     *
     * @details If succeedIfExisting is set to true, the operation would succeed even if the location was already added.
     */
    struct GAMELIFT_ANYWHERE_ADD_FLEET_LOCATION_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetId = nullptr;
        const char* customLocation = nullptr;
        bool succeedIfExisting = false;
    };

    /**
     * @brief Create Anywhere fleet request structure, provided to #GameLiftAnywhereCreateFleet().
     */
    struct GAMELIFT_ANYWHERE_CREATE_FLEET_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetName = nullptr;
        const char* fleetDescription = nullptr;
        const char* customLocation = nullptr;
        const char* tagKey = nullptr;
        const char* tagValue = nullptr;
    };

    /**
     * @brief Anywhere fleet info structure, provided to #GameLiftAnywhereCreateFleet().
     */
    struct GAMELIFT_ANYWHERE_FLEET_INFO {
        const char* fleetId = nullptr;
    };

    /**
     * @brief Anywhere fleet description request structure, provided to #GameLiftAnywhereDescribeFleets().
     */
    struct GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_REQUEST : public GAMELIFT_ERROR_HANDLER {
        int fleetLimit = MAX_ANYWHERE_FLEETS;
    };

    /**
     * @brief Anywhere fleet description info structure, provided to #GameLiftAnywhereDescribeFleets().
     */
    struct GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_INFO {
        struct FleetInfo
        {
            char fleetName[MAX_ANYWHERE_STR_LEN + 1];
            char fleetId[MAX_ANYWHERE_STR_LEN + 1];
            Aws::GameLift::Model::FleetStatus fleetStatus = Aws::GameLift::Model::FleetStatus::NOT_SET;
        };

        FleetInfo fleets[MAX_ANYWHERE_FLEETS];
        int numFleets = 0;
    };

    /**
     * @brief Request Anywhere compute request structure, provided to #GameLiftAnywhereRegisterCompute().
     */
    struct GAMELIFT_ANYWHERE_REGISTER_COMPUTE_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* computeName = nullptr;
        const char* fleetId = nullptr;
        const char* ipAddress = nullptr;
        const char* customLocation = nullptr;
        bool deregisterExistingCompute = true;
    };

    /**
     * @brief Generate Anywhere compute auth token request structure, provided to #GameLiftAnywhereGenerateAuthToken().
     */
    struct GAMELIFT_ANYWHERE_GENERATE_AUTH_TOKEN_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetId = nullptr;
        const char* computeName = nullptr;
    };

    /**
     * @brief Anywhere compute auth token info structure, provided to #GameLiftAnywhereGenerateAuthToken().
     */
    struct GAMELIFT_ANYWHERE_AUTH_TOKEN_INFO {
        const char* authToken = nullptr;
        int64_t milisecondsSinceEpoch = 0;
    };

    /**
     * @brief Create an Anywhere custom location.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param createLocationRequest Pointer to a request input structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereCreateCustomLocation(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_CREATE_LOCATION_REQUEST* createLocationRequest);

    /**
     * @brief Add an Anywhere custom location to a fleet.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param addLocationRequest Pointer to a request input structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereAddFleetLocation(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_ADD_FLEET_LOCATION_REQUEST* addFleetLocationRequest);

    /**
     * @brief Create an Anywhere fleet.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param createFleetRequest Pointer to a request input structure.
     * @param fleetInfo Pointer to a info output structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereCreateFleet(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_CREATE_FLEET_REQUEST* createFleetRequest, GAMELIFT_ANYWHERE_FLEET_INFO* fleetInfo);

    /**
     * @brief Register an Anywhere compute.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param registerComputeRequest Pointer to a request input structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereRegisterCompute(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_REGISTER_COMPUTE_REQUEST* registerComputeRequest);

    /**
     * @brief Generate an Anywhere compute auth token.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param generateAuthTokenRequest Pointer to a request input structure.
     * @param authTokenInfo Pointer to a info output structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereGenerateAuthToken(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_GENERATE_AUTH_TOKEN_REQUEST* generateAuthTokenRequest, GAMELIFT_ANYWHERE_AUTH_TOKEN_INFO* authTokenInfo);

    /**
     * @brief Get a list of Anywhere fleet descriptions.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param describeFleetsRequest Pointer to a request input structure.
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftAnywhereDescribeFleets(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_REQUEST* describeFleetsRequest, GAMELIFT_ANYWHERE_DESCRIBE_FLEETS_INFO* describeFleetsInfo);

#pragma endregion

#pragma region GameLiftClient

    /**
     * @brief Game session info provided by #GameLiftGameSessionGetInfo() and #GameLiftDescribeGameSessions()
    */
    struct GAMELIFT_GAMESESSION_INFO {
        const char* gameSessionId = nullptr;
        const char* name = nullptr;
        const char* fleetId = nullptr;
        const char* fleetArn = nullptr;
        int currentPlayerSessionCount = 0;
        int maximumPlayerSessionCount = 0;
        Aws::GameLift::Model::GameSessionStatus status = Aws::GameLift::Model::GameSessionStatus::NOT_SET;
        Aws::GameLift::Model::GameSessionStatusReason statusReason = Aws::GameLift::Model::GameSessionStatusReason::NOT_SET;
        const char* ipAddress = nullptr;
        const char* dnsName = nullptr;
        int port = 0;
        const char* creatorId = nullptr;
        const char* gameSessionData = nullptr;
        const char* matchmakerData = nullptr;
        const char* location = nullptr;
    };

    /**
     * @brief Game session callback invoked by #GameLiftDescribeGameSessions()
    */
    typedef bool (*FuncGameSessionInfoCallback)(DISPATCH_RECEIVER_HANDLE dispatchReceiver, GAMELIFT_GAMESESSION_INFO* gameSessionInfo);

    /**
     * @brief Describe game sessions request structure, provided to #GameLiftDescribeGameSessions()
    */
    struct GAMELIFT_DESCRIBE_GAMESESSIONS_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetId = nullptr;
        const char* statusFilter = nullptr;
        DISPATCH_RECEIVER_HANDLE dispatchReceiver = nullptr;
        FuncGameSessionInfoCallback resultCb = nullptr;
    };

    /**
     * @brief Create game session request structure, provided to #GameLiftGameSessionCreate()
    */
   struct GAMELIFT_GAMESESSION_CREATE_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetId = nullptr;
        const char* aliasId = nullptr;
        const size_t* maximumPlayerSessionCount  = nullptr;
        const char* name = nullptr;
        const char* creatorId = nullptr;
        const char* gameSessionId = nullptr;
        const char* idempotencyToken = nullptr;
        const char* gameSessionData = nullptr;
        const char* location = nullptr;
    };

    /**
     * @brief Create player session request structure, provided to #GameLiftPlayerSessionCreate()
    */
    struct GAMELIFT_PLAYERSESSION_CREATE_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* gameSessionId = nullptr;
        const char* playerId = nullptr;
        const char* playerData = nullptr;
    };

    /**
     * @brief Player session info provided by #GameLiftPlayerSessionGetInfo
    */
    struct GAMELIFT_PLAYERSESSION_INFO {
        const char *playerSessionId = nullptr;
        const char *playerId = nullptr;
        const char *gameSessionId = nullptr;
        const char *fleetId = nullptr;
        const char *fleetArn = nullptr;
        Aws::GameLift::Model::PlayerSessionStatus status = Aws::GameLift::Model::PlayerSessionStatus::NOT_SET;
        const char *ipAddress = nullptr;
        const char *dnsName = nullptr;
        int port = 0;
        const char *playerData = nullptr;
    };

    /**
     * @brief Create a GameLiftClient instance, which can be used to access the GameLiftClient API.
     *
     * @details Make sure to call GameLiftClientInstanceRelease() to destroy the returned object when finished with it.
     *
     * @return Pointer to the new GameLiftClient instance.
     */
    GAMELIFT_API GAMELIFT_CLIENT_INSTANCE_HANDLE GameLiftClientInstanceCreate();

    /**
     * @brief Create a GameLiftClient instance with credentials, which can be used to access the GameLiftClient API.
     *
     * @details Make sure to call GameLiftClientInstanceRelease() to destroy the returned object when finished with it.
     *
     * @param credentialsName Credentials profile name to set the client instance.
     * @return Pointer to the new GameLiftClient instance.
     */
    GAMELIFT_API GAMELIFT_CLIENT_INSTANCE_HANDLE GameLiftClientInstanceCreateWithCredentials(const char* credentialsName);

    /**
     * @brief Destroy the provided GameLiftClient instance.
     *
     * @param settingsInstance Pointer to a GameLiftClient instance created with GameLiftClientInstanceCreate().
     */
    GAMELIFT_API void GameLiftClientInstanceRelease(GAMELIFT_CLIENT_INSTANCE_HANDLE gameliftClientInstance);

    /**
     * @brief Overrides a GameLiftClient endpoint.
     *
     * @param gameliftClientInstance Pointer to a GameLiftClient instance created with GameLiftClientInstanceCreate().
     * @param endPoint An endpont in format like 'http://127.0.0.1:7777'
     */
    GAMELIFT_API void GameLiftClientOverrideEndpoint(GAMELIFT_CLIENT_INSTANCE_HANDLE gameliftClientInstance, const char* endPoint);

    /**
     * @brief Create a GameLift session and its representation as a GameSession instance that is used to access the GameSession API.
     *
     * @details Make sure to call GameLiftGameSessionRelease() to destroy the returned object when finished with it.
     *
     * @param gameliftClientInstance Pointer to a GameLiftClient instance created with GameLiftClientInstanceCreate().
     * @param createSessionRequest Pointer to a GAMELIFT_GAMESESSION_CREATE_REQUEST input structure
     * @return Pointer to the new GameSession instance.
     */
    GAMELIFT_API GAMELIFT_GAMESESSION_INSTANCE_HANDLE GameLiftGameSessionCreate(GAMELIFT_CLIENT_INSTANCE_HANDLE gameliftClientInstance, GAMELIFT_GAMESESSION_CREATE_REQUEST* createSessionRequest);

    /**
     * @brief Destroy the provided GameSession instance.
     *
     * @details Only GameSession object is destroyed. The GameLift game session itself remains intact.
     *
     * @param gameSessionInstance Pointer to a GameSession object created with GameLiftGameSessionCreate().
     */
    GAMELIFT_API void GameLiftGameSessionRelease(GAMELIFT_GAMESESSION_INSTANCE_HANDLE gameSessionInstance);

    /**
     * @brief Provide the game session id for a GameSession instance.
     *
     * @param gameSessionInstance Pointer to a GameSession object created with GameLiftGameSessionCreate().
     * @return GameSession id.
     */
    GAMELIFT_API const char* GameLiftGameSessionId(GAMELIFT_GAMESESSION_INSTANCE_HANDLE gameSessionInstance);

    /**
     * @brief Provide the extended game session information for a GameSession instance.
     *
     * @param gameSessionInstance Pointer to a GameSession object created with GameLiftGameSessionCreate().
     * @param gameSessionInfo Game session information
     */
    GAMELIFT_API void GameLiftGameSessionGetInfo(GAMELIFT_GAMESESSION_INSTANCE_HANDLE gameSessionInstance, GAMELIFT_GAMESESSION_INFO* gameSessionInfo);

    /**
     * @brief Iterates over all game sessions and provide game session information to a handler from request.
     *
     * @param gameSessionInstance Pointer to a GameSession object created with GameLiftGameSessionCreate().
     * @param describeGameSessionsRequest Describe game session request.
     * @return True if operation succeeded. False - otherwise.
     */
    GAMELIFT_API bool GameLiftDescribeGameSessions(GAMELIFT_CLIENT_INSTANCE_HANDLE gameliftClientInstance, GAMELIFT_DESCRIBE_GAMESESSIONS_REQUEST* describeGameSessionsRequest);

    /**
     * @brief Create a GameLift player session and its representation as a PlayerSession instance that is used to access the PlayerSession API.
     *
     * @details Make sure to call GameLiftPlayerSessionRelease() to destroy the returned object when finished with it.
     *
     * @param gameliftClientInstance Pointer to a GameLiftClient instance created with GameLiftClientInstanceCreate().
     * @param createSessionRequest Pointer to a GAMELIFT_PLAYERSESSION_CREATE_REQUEST input structure.
     * @return Pointer to the new PlayerSession instance.
     */
    GAMELIFT_API GAMELIFT_PLAYERSESSION_INSTANCE_HANDLE GameLiftPlayerSessionCreate(GAMELIFT_CLIENT_INSTANCE_HANDLE gameliftClientInstance, GAMELIFT_PLAYERSESSION_CREATE_REQUEST* createSessionRequest);

    /**
     * @brief Destroy the provided PlayerSession instance.
     *
     * @details Only PlayerSession object is destroyed. The GameLift player session itself remains intact.
     *
     * @param playerSessionInstance Pointer to a PlayerSession object created with GameLiftPlayerSessionCreate().
     */
    GAMELIFT_API void GameLiftPlayerSessionRelease(GAMELIFT_PLAYERSESSION_INSTANCE_HANDLE playerSessionInstance);

    /**
     * @brief Provide the player session id for a PlayerSession instance.
     *
     * @param playerSessionInstance Pointer to a PlayerSession object created with GameLiftPlayerSessionCreate().
     * @return PlayerSession id.
     */
    GAMELIFT_API const char* GameLiftPlayerSessionId(GAMELIFT_PLAYERSESSION_INSTANCE_HANDLE playerSessionInstance);

    /**
     * @brief Provide extended player session information.
     *
     * @param playerSessionInstance Pointer to a PlayerSession object created with GameLiftPlayerSessionCreate().
     * @param playerSessionInfo Player session information.
     */
    GAMELIFT_API void GameLiftPlayerSessionGetInfo(GAMELIFT_PLAYERSESSION_INSTANCE_HANDLE playerSessionInstance, GAMELIFT_PLAYERSESSION_INFO* playerSessionInfo);

#pragma endregion

#pragma region GameLiftECR
    /**
     * @brief Repo name and uri from describe repositories call
     */
    struct GAMELIFT_ECR_DESCRIBE_REPO_INFO {
        struct RepoInfo
        {
            char repoName[MAX_REPO_STR_LEN + 1];
            char repoUri[MAX_REPO_STR_LEN + 1];
        };

        RepoInfo repos[MAX_REPO_COUNT];
        int numRepos = 0;
    };

    /**
     * @brief Image digest and tag from list iamges call
     */
    struct GAMELIFT_ECR_LIST_IMAGES_INFO {
        struct ImageInfo
        {
            char imageDigest[MAX_IMAGE_STR_LEN + 1];
            char imageTag[MAX_IMAGE_STR_LEN + 1];
        };

        ImageInfo images[MAX_IMAGE_COUNT];
        int numImages = 0;
    };

    /**
     * @brief ERC Repo uri
     */
    struct GAMELIFT_ECR_REPO_URI {
        char repoUri[MAX_REPO_STR_LEN + 1];
    };

    /**
     * @brief Error info passed to a error handler callback #ECRFuncErrorCallback when calling ECR APIs.
     */
    struct ECR_ERROR_INFO {
        Aws::ECR::ECRErrors errorType = Aws::ECR::ECRErrors::UNKNOWN;
        const char* errorMessage = nullptr;
    };

    /**
     * @brief Error handler callback. Refer to #ECR_ERROR_HANDLER.
     */
    typedef void (*ECRFuncErrorCallback)(DISPATCH_RECEIVER_HANDLE errorReceiver, ECR_ERROR_INFO* errorInfo);

    /**
     * @brief Error handler passed as a part of request when calling ECR APIs.
     */
    struct ECR_ERROR_HANDLER {
        ECRFuncErrorCallback errorCb = nullptr;
        DISPATCH_RECEIVER_HANDLE errorReceiver = nullptr;
    };

    /**
     * @brief Create describe ecr repositories request structure, provided to #GameLiftECRDescribeRepositories().
     */
    struct GAMELIFT_ECR_DESCRIBE_REPOSITORIES_REQUEST : public ECR_ERROR_HANDLER {
    };

    /**
     * @brief Create list ecr images request structure, provided to #GameLiftECRListImages().
     */
    struct GAMELIFT_ECR_LIST_IMAGES_REQUEST : public ECR_ERROR_HANDLER {
        const char* repositoryName = nullptr;
    };

    /**
     * @brief Create create ecr repository request structure, provided to #GameLiftECRCreateRepository().
     */
    struct GAMELIFT_ECR_CREATE_REPO_REQUEST : public ECR_ERROR_HANDLER {
        const char* repositoryName = nullptr;
    };

    /**
     * @brief Create a ECR private Repository
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param createRepositoryRequest Pointer to a request input structure.
     * @param repoUri structure to be filled by uri of created repository
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftECRCreateRepository(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ECR_CREATE_REPO_REQUEST* createRepositoryRequest, GAMELIFT_ECR_REPO_URI* repoUri);

    /**
     * @brief Describes image repositories in a registry.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param describeRepositoriesRequest Pointer to a request input structure.
     * @param describeRepoInfo structure to be filled by list of repositories' names and uris
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftECRDescribeRepositories(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ECR_DESCRIBE_REPOSITORIES_REQUEST* describeRepositoriesRequest, GAMELIFT_ECR_DESCRIBE_REPO_INFO* describeRepoInfo);

    /**
     * @brief List all images that exist in the specified ecr repositories with current AWS account for the selected region.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param listImagesRequest Pointer to a request input structure.
     * @param listImageInfo structure to be filled by list of images' info
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftECRListImages(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_ECR_LIST_IMAGES_REQUEST* listImagesRequest, GAMELIFT_ECR_LIST_IMAGES_INFO* listImageInfo);

#pragma endregion

#pragma region GameLiftContainers
    /**
     * @brief Containers list cgd info structure, provided to #GameLiftContainersListCgds().
     */
    struct GAMELIFT_CONTAINERS_LIST_CGD_INFO {
        struct CgdInfo
        {
            char cgdName[MAX_CGD_STR_LEN + 1];
        };

        CgdInfo cgds[MAX_CGD_COUNT];
        int numCgds = 0;
    };

    /**
     * @brief Containers describe cgd info structure, provided to #GameLiftContainersDescribeCgd().
     */
    struct GAMELIFT_CONTAINERS_DESCRIBE_CGD_INFO {
        char cgdStatus[MAX_CGD_STATUS_STR_LEN + 1];
        int cgdVersion = 0;
    };

    /**
     * @brief Containers describe container fleet info structure, provided to #GameLiftContainersDescribeContainerFleet().
     */
    struct GAMELIFT_CONTAINERS_DESCRIBE_CONTAINER_FLEET_INFO {
        char fleetId[MAX_FLEETID_STR_LEN + 1];
        Aws::String instanceType;
        Aws::GameLift::Model::ContainerFleetStatus containerFleetStatus = Aws::GameLift::Model::ContainerFleetStatus::NOT_SET;
        Aws::GameLift::Model::ContainerFleetBillingType containerFleetBillingType = Aws::GameLift::Model::ContainerFleetBillingType::NOT_SET;
    };

    /**
     * @brief Create Containers list cgd request structure, provided to #GameLiftContainersListCgds().
     */
    struct GAMELIFT_CONTAINERS_LIST_CGDS_REQUEST : public GAMELIFT_ERROR_HANDLER {
    };

    /**
     * @brief Create describe cgd request structure, provided to #GameLiftContainersDescribeCgd().
     */
    struct GAMELIFT_CONTAINERS_DESCRIBE_CGD_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* cgdName = nullptr;
    };

    /**
     * @brief Create describe container fleet request structure, provided to #GameLiftContainersDescribeContainerFleet().
     */
    struct GAMELIFT_CONTAINERS_DESCRIBE_CONTAINER_FLEET_REQUEST : public GAMELIFT_ERROR_HANDLER {
        const char* fleetId = nullptr;
    };

    /**
     * @brief Retrieves information on all container group definitions that exist in the current AWS account for the selected region.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param listCgdsRequest Pointer to a request input structure.
     * @param listCgdInfo structure to be filled by list of cgds' names
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftContainersListCgds(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_CONTAINERS_LIST_CGDS_REQUEST* listCgdsRequest, GAMELIFT_CONTAINERS_LIST_CGD_INFO* listCgdInfo);


    /**
     * @brief Retrieves the properties of a container group definition, including all container definitions in the group.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient.
     * @param describeCgdRequest Pointer to a request input structure.
     * @param describeCgdInfo structure to be filled by specified cgd's status
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftContainersDescribeCgd(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_CONTAINERS_DESCRIBE_CGD_REQUEST* describeCgdRequest, GAMELIFT_CONTAINERS_DESCRIBE_CGD_INFO* describeCgdInfo);

    /**
     * @brief Retrieves the properties of a container fleet.
     *
     * @param accountInstance Pointer to a GameLiftAccount instance that contains a GameLiftClient. Might need to make sure the game name is set to ensure it gets correct stack name
     * @param describeContainerFleetRequest Pointer to a request input structure.
     * @param describeContainerFleetInfo structure to be filled by specified container fleet's status
     * @return true if the operation succeeds.
     */
    GAMELIFT_API bool GameLiftContainersDescribeContainerFleet(GAMELIFT_ACCOUNT_INSTANCE_HANDLE accountInstance, GAMELIFT_CONTAINERS_DESCRIBE_CONTAINER_FLEET_REQUEST* describeContainerFleetRequest, GAMELIFT_CONTAINERS_DESCRIBE_CONTAINER_FLEET_INFO* describeContainerFleetInfo);

#pragma endregion
}
