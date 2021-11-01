function loadIndexView(){
   document.getElementById(GLOBALS.HTML.MAIN.TITLE).innerHTML = "Home"

   var breadcrumb = {};
   breadcrumb.active_last = true;
   breadcrumb.map = [];
   breadcrumb.map.push({"name": "Home", "where_to": ""});
   generateTrail(breadcrumb);

   createCardsForInstitutionList();
}

function createCardsForInstitutionList(){

   document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = "";
   var institution_list = sessionStorage.getItem(GLOBALS.SESSION_KEYS.USER_INSTITUTIONS);
   institution_list = JSON.parse(institution_list);
   if (institution_list == null) return;
   if (institution_list.length < 1) return; 

   const CARD = '<div class="col-xl-2 col-md-4"></div> \
                 <div class="col-xl-8 col-md-4"> \
                     <div class="card bg-primary text-white mb-4" style = "cursor: pointer;" onclick="##WHATTODO##"> \
                         <div class="card-body vm-institution-card">##NAME##</div> \
                     </div> \
                 </div> \
                 <div class="col-xl-2 col-md-4"></div>'

   var html = '<div class="row">'

   for (var i = 0; i < institution_list.length; i++){
      var card = CARD;

      card = card.replace("##NAME##",institution_list[i][GLOBALS.INST_LIST.NAME]);
      card = card.replace("##WHATTODO##","goToInstitution("+  institution_list[i][GLOBALS.INST_LIST.ID] + ",'" + GLOBALS.ROUTING.PAGES.SUBJECTS + "')");

      //console.log(card);

      html = html + card + "\n";
   }

   html = html + "</div>" // Closing the row;

   

   document.getElementById(GLOBALS.HTML.MAIN.CONTENTS).innerHTML = html;
}
