          
function attemptLogin() {
    username = "gfernandez@gmail.com";
    password = "banana";
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
           if ("data" in this.response){
              Token.setToken(this.response.data["token"]);
              window.location.href = "index.html"
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER,username);
              sessionStorage.setItem(GLOBALS.SESSION_KEYS.USER_ID,this.response.data["id"]);
           }
           else{
              // Show message error response.message. 
           }
        }
    };
    xhr.send(null);
}