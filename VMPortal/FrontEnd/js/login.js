          
function attemptLogin() {
    username = document.getElementById("email").value;
    password = document.getElementById("password").value;
    login(username,password);
}

function login(username,password) {
    var xhr = new XMLHttpRequest();
    xhr.open('POST', sessionStorage.getItem(GLOBALS.SESSION_KEYS.API), true);
    
    xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTH_TYPE,
                         GLOBALS.HEADERS_VALUES.LOGIN);

    xhr.setRequestHeader(GLOBALS.HEADERS_NAMES.AUTHORIZATION, 
                         GLOBALS.HEADERS_VALUES.BASIC + " " + btoa(username + ":"  + password));
    
    xhr.responseType = "json";
    
    xhr.onreadystatechange = function() {
        if (xhr.readyState == 4) {
           if (this.response.message == "OK"){
              Token.setToken(this.response.data["token"]);              
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER,username);
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_ID,this.response.data["id"]);
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_FNAME,this.response.data["fname"]);
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_LNAME,this.response.data["lname"]);
              WaitDialog.open("Loading ...");
              API.listInstitutionsForUser("receivedInstitutionInfo")
           }
           else{
              document.getElementById("error_message").innerHTML = this.response.message;
           }
        }
    };
    xhr.send(null);
}


function receivedInstitutionInfo(data){

    //console.log(JSON.stringify(data));

    if (data.message != "OK"){
        document.getElementById("error_message").innerHTML = data.message;
        WaitDialog.close();
        return;
    }

    ilist = data.data;
    var inst_list = [];
    for (var i = 0; i < ilist.length; i++){
        var inst = {};
        inst[GLOBALS.INST_LIST.NAME] =  ilist[i]["institution_name"];
        inst[GLOBALS.INST_LIST.ID]   =  ilist[i]["keyid"];
        inst_list.push(inst);
    }

    sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_INSTITUTIONS,JSON.stringify(inst_list));

    // Making sure we are home.
    WaitDialog.close();
    window.location.href = "index.html"

}
