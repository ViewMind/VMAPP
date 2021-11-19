function loadViewAccountData(){

   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Account Information";

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": "index.html"});
   breadcrumb.map.push({"name": "My Account", "where_to": ""});
   generateTrail(breadcrumb);   

   var fname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_FNAME);
   var lname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_LNAME);

   var html = '\
   <div class="row justify-content-center">\
      <div class="col-lg-5">\
          <div class="card shadow-lg border-0 rounded-lg mt-5">\
              <div class="card-header vm-gray">\
                 Modify Account Information\
              </div>\
              <div class="card-body">\
                  <form>\
                      <div class="form-floating mb-3">\
                          <input class="form-control" id="fname" type="text" value ="' + fname + '"/>\
                          <label for="inputEmail">First Name</label>\
                      </div>\
                      <div class="form-floating mb-3">\
                          <input class="form-control" id="lname" type="text" value ="' + lname + '"/>\
                          <label for="inputEmail">First Name</label>\
                      </div>\
                      <div class="form-floating mb-3">\
                          <input class="form-control" id="password" type="password" placeholder="Password"/>\
                          <label for="inputPassword">Password</label>\
                      </div>\
                      <div class="form-floating mb-3">\
                          <input class="form-control" id="cpassword" type="password" placeholder="Confirm Password"/>\
                          <label for="inputPassword">Confirm Password</label>\
                      </div>\
                      <div class="d-flex align-items-center justify-content-between mt-4 mb-0">\
                          <a class="btn btn-primary" style="width: 100%;" onclick="saveChanges()">Save Changes</a>\
                      </div>\
                  </form>\
                  <div class="mt-4 mb-0" style="text-align: center;">\
                      <p id = "error_message" style="color: #dc3545; font-weight: bold;"></p>\
                  </div>\
              </div>\
          </div>\
      </div>\
   </div>'

   document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = html;

}

function saveChanges(){

   var fname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_FNAME);
   var lname = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_LNAME);

   var to_validate = {
       fname: "First Name",
       lname: "Last Name"
   };

   for (key in to_validate){
      let name = to_validate[key];
      if (!noSpecialCharsInput(document.getElementById(key).value)){
        ErrorDialog.open("Field: " + name,"Invalid characters used. Make sure to only use letters and numbers");
        return;
      }
   }

   var tomod = {};
   var nfname = document.getElementById("fname").value;
   var nlname = document.getElementById("lname").value;
   var passwd = document.getElementById("password").value;
   var cpasswd = document.getElementById("cpassword").value;

   if ((passwd != "") || (cpasswd != "")){
      if (!checkPassword(passwd)){
         ErrorDialog.open("Invalid Password","<p>The password must meet the following requirements<ul> <li> Between 8 and 64 caracters </li> <li> Must contains at least 1 uppercase, 1 lower case, 1 number and 1 special character </li>  <li>Must not contain the username </li> </ul></p>");
         return;
      }
   }

   if (nfname == ""){
      ErrorDialog.open("Input Error","First name cannot be empty");
      return;
   }

   if (nlname == ""){
      ErrorDialog.open("Input Error","Last name cannot be empty");
      return;
   }

   if (nfname != fname) {
      tomod[ACCESS.PORTALUSER.NAME] = nfname;
   }

   if (nlname != lname){
      tomod[ACCESS.PORTALUSER.LASTNAME] = nlname
   }

   if ((passwd != "") || (cpasswd != "")){
       if (passwd != cpasswd){
          ErrorDialog.open("Password mistmatch","The password and confirmation password are not the same");
          return;           
       }
       else{
           tomod[ACCESS.PORTALUSER.PASSWD] = passwd;
       }
   }

   if (tomod.length == 0){
      ErrorDialog.open("Nothing to do","No information was changed");
      return;
   }

   API.modifyAccount(tomod,"onModFinished");

}

function checkPassword(password){

    var username = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER);
    var pattern = new RegExp(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[-+_!@#$%^&*.,?]).+$"
    );

    // Checking if the password contains the username
    if (password.includes(username)){
        return false;
    }

    // Checking the length
    if ((password.length < 8) || (password.length > 64)){
        return false;
    }

    return pattern.test(password);
}


function onModFinished(response){

    if (response.message != "OK"){
        ErrorDialog.open("Error","Error modifying data: " + response.message);
        return;
    }
    else{
        ErrorDialog.open("Success","Your modifications were successfull");

        sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_FNAME, document.getElementById("fname").value);
        sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_LNAME, document.getElementById("lname").value);
        setUserName();
     
        window.location.href = "index.html";
    }

}