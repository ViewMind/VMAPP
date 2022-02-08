<?php

include_once ("ObjectBaseClass.php");
include_once (__DIR__ . "/../db_management/DBCon.php");
include_once (__DIR__ . "/../db_management/TablePortalUsers.php");
include_once (__DIR__ . "/../db_management/TableInstitutionUsers.php");

class ObjectPortalUsers extends ObjectBaseClass{

   const MODPERM_ID = "id";
   const MODPERM_ACTION = "action";
   const MODPERM_PERMISSIONS = "permissions";
   const MODPERM_ROLE = "role";
   const MODPERM_PERMISSION_LIST = "permission_list";
   
   const BASE_USER_PERMISSION_PORTAL        = "portal";
   const BASE_USER_PERMISSION_ADMIN_PORTAL  = "portal-admin";
   const BASE_USER_PERMISSION_CLEAN         = "clean";

   const FORCE_ENABLE = "force_enable";

   function __construct($service,$headers){
      parent::__construct($service,$headers);
   }

   function getallmedical($identifier,$parameters){

      if ($identifier == ""){
         $this->error = "Empty institution identifier.";
         return false;
      }

      // WE first need to get the list of IDs form the Institution - User ID table. 
      $tiu = new TableInstitutionUsers($this->con_main);
      $ans = $tiu->getUsersForInstitution($identifier);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get information on the users for this institution";
         $this->error = "Getting ids for institution $identifier users: " . $tiu->getError();
         return false;
      }
      // If the answer is empty, it is just that there are no users for this institution 
      if (empty($ans)){
         return array();
      }

      // Transforming result into a list of ids. 
      $user_ids = array();
      foreach ($ans as $row){
         $user_ids[] = $row[TableInstitutionUsers::COL_PORTAL_USER];
      }

      // Then we map those to the info. 
      $tpu = new TablePortalUsers($this->con_secure);
      $enabled_status = [TablePortalUsers::ENABLED, TablePortalUsers::NOLOG];
      $ans = $tpu->getAllNamesForIDList($user_ids,$enabled_status);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->returnable_error = "Could not get information on the users for this institution";
         $this->error = "Getting names for userids for institution $identifier: " . $tpu->getError();
         return false;
      }

      // We need to filter out the users that cannot login into the portal. 
      $users = array();
      foreach ($ans as $row){
         $role = intval($row[TablePortalUsers::COL_USER_ROLE]);
         if (LoginMasks::canLoginToPortal($role)){
            $users[] = $row;
         }
      }

      //echoOut($ans,true);

      // // We create the final structure.
      // $ret = array();
      // foreach ($ans as $row){
      //    $ret[$row[TablePortalUsers::COL_KEYID]] = array();
      //    $ret[$row[TablePortalUsers::COL_KEYID]]["name"] = $row[TablePortalUsers::COL_NAME] . " " . $row[TablePortalUsers::COL_LASTNAME];
      //    $ret[$row[TablePortalUsers::COL_KEYID]]["email"] = $row[TablePortalUsers::COL_EMAIL];
      // }

      return $ans;

   }

   function modify_own($identifier,$parameters){

      // The portal user that requests the reports for this subject. 
      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      // The identifier is ignored. As we can trust the $portal_user. 

      // This was done because the information used to be part of the URL and no it is part of the body of the request. 
      $parameters = $this->json_data;

      // Checking that only the allowed columns are modified. 
      $allowed_cols = [ TablePortalUsers::COL_COMPANY, TablePortalUsers::COL_PASSWD, TablePortalUsers::COL_NAME, TablePortalUsers::COL_LASTNAME ];
      foreach ($parameters as $key => $value){
         if (!in_array($key,$allowed_cols)){
            $this->suggested_http_code = 401;
            $this->error = "When modifying user $portal_user, cannot specify value for $key";
            $this->returnable_error = "Parameter error in API call";
            return false;            
         }
      }

      // Hashing the password if present. 
      if (array_key_exists(TablePortalUsers::COL_PASSWD,$parameters)){
         $password = password_hash($parameters[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);
         $parameters[TablePortalUsers::COL_PASSWD] = $password;
      }

      // Updating the table. 
      $tpu = new TablePortalUsers($this->con_secure);
      $ans = $tpu->updateOwnUser($portal_user,$parameters);
      if ($ans === FALSE){
         $this->suggested_http_code = 500;
         $this->error = "When modifying user $portal_user, update failed. Reason: " . $tpu->getError();
         $this->returnable_error = "Internal Server Error";
         return false;         
      }

      return array();

   }

   /**
    * General administrative function that allows creation, modification and password reset for a given user. All using the request body.
    */
   function operate($identifier,$parameters){

      // Action is always mandatory. 
      if (!array_key_exists(self::MODPERM_ACTION,$this->json_data)){
         $this->suggested_http_code = 401;
         $this->error = "The request body must contain the 'action' field";
         return false;            
      }

      // Creating the table elements.
      $tpu = new TablePortalUsers($this->con_secure);
      $ti = new TableInstitution($this->con_main);
      $tiu = new TableInstitutionUsers($this->con_main);

      $action = $this->json_data[self::MODPERM_ACTION];
      if ($action === EndpointBodyActions::CREATE){

         /**
          * In this case the requiremets are
          *   Name
          *   Lastname
          *   Email (this will be ignored in a update)
          *   Password
          *   Role Mask
          *   Company. 
          *   Institution ID. 
          *   Force enable. 
          *   Base permission set (Options are portal, admin or clean)
          */

         $required_fields = [
            TablePortalUsers::COL_NAME,
            TablePortalUsers::COL_LASTNAME,
            TablePortalUsers::COL_EMAIL,
            TablePortalUsers::COL_PASSWD,
            TablePortalUsers::COL_USER_ROLE,
            TablePortalUsers::COL_COMPANY,
            TableInstitutionUsers::COL_INSTITUTION_ID,
            TablePortalUsers::COL_PERMISSIONS,
            self::FORCE_ENABLE,
         ];

         foreach ($required_fields as $reqfield){
            if (!array_key_exists($reqfield,$this->json_data)){
               $this->suggested_http_code = 401;
               $this->error = "The request body must contain the '$reqfield' field when creating a new portal user";
               return false;                  
            }
         }

         // Verifying the instituion exists
         $institution_id = $this->json_data[TableInstitutionUsers::COL_INSTITUTION_ID];

         if (!$ti->exists($institution_id)){
            $this->suggested_http_code = 401;
            $this->error = "When creating a portal user, the institution id $institution_id does not exist";
            return false;                  
         }

         // Hashing the password.
         $password = password_hash($this->json_data[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);

         // Getting the permissions.
         $permtype = $this->json_data[TablePortalUsers::COL_PERMISSIONS];
         $permissions = array();
         if ($permtype === self::BASE_USER_PERMISSION_ADMIN_PORTAL){
            $permissions = TablePortalUsers::getAdminPermissions();
         }
         else if ($permtype === self::BASE_USER_PERMISSION_PORTAL){
            $permissions = TablePortalUsers::getStandardPermissions();
         }
         else if ($permtype === self::BASE_USER_PERMISSION_CLEAN){
            $permissions = TablePortalUsers::getCleanPermissions();
         }
         else {
            $this->suggested_http_code = 401;
            $this->error = "Unknown base permision set when creating portal user: $permtype";
            return false;                  
         }
         
         $force_enabled = $this->json_data[self::FORCE_ENABLE];

         // Copying all straight data to a clean array. 
         $to_copy = [
            TablePortalUsers::COL_NAME,
            TablePortalUsers::COL_LASTNAME,
            TablePortalUsers::COL_EMAIL,
            TablePortalUsers::COL_USER_ROLE,
            TablePortalUsers::COL_COMPANY,
         ];

         $params = array();
         foreach ($to_copy as $key){
            $params[$key] = $this->json_data[$key];
         }

         $params[TablePortalUsers::COL_PERMISSIONS] = json_encode($permissions);
         $params[TablePortalUsers::COL_PASSWD] = $password;
         
         // Finally creating the new user. 
         $ans = $tpu->addPortalUser($params, $force_enabled);

         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in adding portal user. Reason: " . $tpu->getError();
            $this->returnable_error = "Failed in database operation for adding new user";
            return false;                  
         }

         // If not force enabled we need to return the creation token. Otherwise we return empty.
         $ret["token"] = "";

         if (!$force_enabled){
            $ret["token"] = $ans[0];
         }

         // Finally we need to insert the user into the institution users table. 
         $user_id = $tpu->getLastInserted()[0];
         $ans = $tiu->linkUserToInstitution($user_id,$institution_id);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in database operation for liking user $user_id to institution $institution_id. Reason: " . $tiu->getError();
            $this->returnable_error = "Failed in database operation for liking user $user_id to institution $institution_id";
            return false;                  
         }

         return $ret;

      }
      else if ($action === EndpointBodyActions::SET){

         /**
          * Here only the id is mandatory and all modifiable parameters are not. If they are present and not empty
          * they are modified. Otherwise they are left alone. The only parameters taht can be modified by this operation are
          *   Name
          *   Last name
          *   Company.
          *   Password. 
          */

         if (!array_key_exists(self::MODPERM_ID,$this->json_data)){
            $this->suggested_http_code = 401;
            $this->error = "The request body must contain the 'id' field when modifying a portal user";
            return false;                  
         }
         $user = $this->json_data[self::MODPERM_ID];

         $to_check = [
            TablePortalUsers::COL_NAME,
            TablePortalUsers::COL_LASTNAME,
            TablePortalUsers::COL_COMPANY,
         ];         
         
         $params = [];
         foreach ($to_check as $key){
            if (array_key_exists($key,$this->json_data)){
               $params[$key] = $this->json_data[$key];
            }
         }

         // Checking if we are resetting the passwor.d
         if (array_key_exists(TablePortalUsers::COL_PASSWD, $this->json_data)) {
            $params[TablePortalUsers::COL_PASSWD] = password_hash($this->json_data[TablePortalUsers::COL_PASSWD], PASSWORD_BCRYPT, ["cost" => 10]);
         }

         // Finally checking if we are modifying anything.
         if (count($params) == 0){
            $this->suggested_http_code = 401;
            $this->error = "The request body for modifying portal user $user contained no modifiable fields";
            return false;                  
         }

         //If all is good we do the modification. 
         $ans = $tpu->updateOwnUser($user,$params);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in modifying portal user $user. Reason: " . $tpu->getError();
            $this->returnable_error = "Failed in database operation for modifying user $user";
            return false;                  
         }

         return array();

      }
      else if ($action === EndpointBodyActions::GET){

         if (!array_key_exists(self::MODPERM_ID,$this->json_data)){
            $this->suggested_http_code = 401;
            $this->error = "The request body must contain the 'id' field when getting a portal user";
            return false;                  
         }
         $user = $this->json_data[self::MODPERM_ID];

         $ans = $tpu->getInfoForUser($user,true);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in getting portal user $user. Reason: " . $tpu->getError();
            $this->returnable_error = "Failed in database operation for retrieving user $user";
            return false;                  
         }

         $to_return = $ans[0];

         // Removing some fields
         unset($to_return[TablePortalUsers::COL_PASSWD]);
         unset($to_return[TablePortalUsers::COL_TOKEN]);
         unset($to_return[TablePortalUsers::COL_TOKEN_EXPIRATION]);

         return $to_return;

      }
      else if ($action === EndpointBodyActions::LINK){
         
         // This will simply link a user to an institution. 
         $required_fields = [
            TableInstitutionUsers::COL_INSTITUTION_ID,
            TableInstitutionUsers::COL_PORTAL_USER
         ];

         foreach ($required_fields as $reqfield){
            if (!array_key_exists($reqfield,$this->json_data)){
               $this->suggested_http_code = 401;
               $this->error = "The request body must contain the '$reqfield' field when linking a portal user to an institution";
               return false;                  
            }
         }

         $user_id = $this->json_data[TableInstitutionUsers::COL_PORTAL_USER];
         $inst_id = $this->json_data[TableInstitutionUsers::COL_INSTITUTION_ID];
         $ans = $tiu->linkUserToInstitution($user_id,$inst_id);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in linking institution $inst_id and user $user_id. Reason: " . $tiu->getError();
            $this->returnable_error = "Failed in database operation for linking user and institution";
            return false;                  
         }

         return array();

      }
      else if ($action === EndpointBodyActions::LIST){

         $ans = $tpu->listEnabledUsers();
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in listing enabled users. Reason: " . $tpu->getError();
            $this->returnable_error = "Failed in database operation for listing portal users";
            return false;                  
         }

         return $ans;

      }
      else {
         $this->suggested_http_code = 401;
         $this->error = "Unknwon action $action when operating on portal users";
         return false;                  
      }

   }

   function logout($identifier,$parameters){

      $auth = $this->headers[HeaderFields::AUTHORIZATION];
      $auth = explode(":",$auth);
      $portal_user = $auth[0];

      $tpu = new TablePortalUsers($this->con_secure);
      //error_log("Logging out user $portal_user");
      $ans = $tpu->invalidateUserToken($portal_user);
      if ($ans === false){
         $this->suggested_http_code = 500;
         $this->error = "Could not clear token for user $portal_user. Reason: " . $tpu->getError();
         $this->returnable_error = "Internal server error";
         return false;
      }

      return array();

   }

   function addnologpusers($identifier,$parameters){

      // First we make sure that the institution idenfier matches the signature institution.
      if (!array_key_exists(POSTFields::INSTITUTION_ID,$this->json_data)){
         $this->suggested_http_code = 401;
         $this->error = "Missing Institution ID in request data";
         $this->returnable_error = "Missing request data";
         return false;         
      }

      $institution_id = $this->json_data[POSTFields::INSTITUTION_ID];
      if ($institution_id != $identifier){
         $this->suggested_http_code = 401;
         $this->error = "Institution in URL was $identifier but the sign in was done with institution $institution_id";
         $this->returnable_error = "Badly formed URL";
         return false;         
      }

      if (!array_key_exists("data",$this->json_data)){
         $this->suggested_http_code = 401;
         $this->error = "Missing 'data' field in the  in request data";
         $this->returnable_error = "Missing request data";
         return false;         
      }      

      $medics = $this->json_data["data"];
      if (!is_array($medics)){
         $this->suggested_http_code = 401;
         $this->error = "The 'data' field in the request data is not an array";
         $this->returnable_error = "Bad request data";
         return false; 
      }

      // Thes are the columns expected in each item of the array;
      //$columns = [TablePortalUsers::COL_EMAIL, TablePortalUsers::COL_NAME, TablePortalUsers::COL_LASTNAME, TablePortalUsers::COL_PARTNER_ID];
      $tpu = new TablePortalUsers($this->con_secure);
      $tiu = new TableInstitutionUsers($this->con_main);

      $ret["inserted"] = array();

      foreach ($medics as $medic){

         //var_dump($medic);

         $ans = $tpu->addNonLoginParterUsers($medic);
         if ($ans === FALSE){
            $this->suggested_http_code = 500;
            $this->error = "Error inserting non login partner user. Reason: " . $tpu->getError();
            $this->returnable_error = "Internal server error";
            return false;    
         }
         
         $insert_id = $tpu->getLastInserted()[0];
         if ($insert_id == 0) continue; // The user has already been created. 

         $ans = $tiu->linkUserToInstitution($insert_id,$institution_id);
         if ($ans === FALSE){
            $this->suggested_http_code = 500;
            $this->error = "Error linking non login partner user $insert_id to institution $institution_id. Reason: " . $tiu->getError();
            $this->returnable_error = "Internal server error";
            return false;    
         }

         $ret["inserted"][] = $medic[TablePortalUsers::COL_EMAIL];

      }

      return $ret;

   }

   function modify_permissions($identifier,$parameters){

      // This endpoint requires information passed in the body of the request. As such the identifier should always be set to 0 as it will not be used. 
      if ($identifier != 0){
         $this->suggested_http_code = 401;
         $this->error = "The expected identifier for modify permissions is 0. But got an $identifier instead";
         $this->returnable_error = "Badly formed modify permissions URL";
         return false;               
      }

      // Veryfing the structure of the request body. Both the id and the action must be present. 
      $must_exist = [self::MODPERM_ID, self::MODPERM_ACTION];

      foreach ($must_exist as $key){
         if (!array_key_exists($key, $this->json_data)){
            $this->suggested_http_code = 401;
            $this->error = "Bad request body. Missing mandatory parameter $key";
            return false;                  
         }
      }

      // Creating the connection. 
      $tpu = new TablePortalUsers($this->con_secure);

      // The ID is required for all operation. For convenience the email is used instead of the actual number ID. 
      $id = $this->json_data[self::MODPERM_ID];

      // If the action is modify then the new set of permissions must be present. 
      if ($this->json_data[self::MODPERM_ACTION] == EndpointBodyActions::SET){

         $must_exist = [self::MODPERM_PERMISSIONS, self::MODPERM_ROLE];

         foreach ($must_exist as $key){
            if (!array_key_exists($key, $this->json_data)){
               $this->suggested_http_code = 401;
               $this->error = "Bad request body. Missing mandatory parameter $key when action is 'set'";
               return false;                  
            }
         }

         $permissions = json_encode($this->json_data[self::MODPERM_PERMISSIONS]);
         $role        = $this->json_data[self::MODPERM_ROLE];

         //var_dump($permissions);

         $ans = $tpu->setUsersPermission($id,$permissions,$role);
         if ($ans === false){
            $this->suggested_http_code = 500;
            $this->error = "Could not set permissions for user $id. Reason: " . $tpu->getError();
            $this->returnable_error = "Internal database error";
            return false;
         }

         // All is good we retrun an empty array();
         return array();

      }
      else if ($this->json_data[self::MODPERM_ACTION] != EndpointBodyActions::GET){
         $this->suggested_http_code = 401;
         $this->error = "Bad action in request body: " . $this->json_data[self::MODPERM_ACTION] . ". Expected either 'set' or 'get'";
         return false;                  
      }
      else {

         // This is simple get.         

         $info = $tpu->getInfoForUser($id,true);
         if ($info === false){
            $this->suggested_http_code = 500;
            $this->error = "Failed in getting information for user $id. Reason: " . $tpu->getError();
            $this->returnable_error = "Internal database error";
            return false;
         }

         if (count($info) != 1){
            $this->suggested_http_code = 500;
            $this->error = "Failed in getting information for user $id. Reason returned rows matching id $id were " . count($info);
            $this->returnable_error = "Internal database error";
            return false;
         }

         $info = $info[0];
         $permissions = json_decode($info[TablePortalUsers::COL_PERMISSIONS],true);
         $role        = $info[TablePortalUsers::COL_USER_ROLE];

         // Creating the list of possible permissions.
         $list = array();
         foreach (ROUTING as $name => $class_name){
            $class = new ReflectionClass($class_name);
            $methods = $class->getMethods(ReflectionMethod::IS_PUBLIC);     
            $list[$name] = array();
            foreach ($methods as $method){
               
               // Methods is a list of objects, not array elments. We need to access its values by using the object -> operaton. The methods will include the constructor and the functions from the base class. 
               $method_name = $method->getName();
               $method_class_name = $method->class;
               

               if ($class_name != $method_class_name) continue; // This will filter functions from the base class
               if ($method_name == "__construct") continue; // This will filter the constructor method. 
               
               //echo "Function $method_class_name.$method_name";
               //echo "=========\n";

               $list[$name][] = $method_name;
            }
         }

         $ret[self::MODPERM_ROLE] = $role;
         $ret[self::MODPERM_PERMISSIONS] = $permissions;
         $ret[self::MODPERM_PERMISSION_LIST] = $list;
         
         return $ret;

      }

   }

}

?>
