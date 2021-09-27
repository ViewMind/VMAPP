var currentSortIndex = ""
var modelList = [];
var currentSortOrder = "descending";
var keyList = [];
var iterator = 0;

const ORDER_ASCENDING = "ascending"
const ORDER_DESCENDING = "descending"

function toggleSortOrder() {
    if (currentSortOrder === ORDER_DESCENDING) currentSortOrder = ORDER_ASCENDING;
    else currentSortOrder = ORDER_DESCENDING;
}

function setSortOrderByBoolean(is_up){
    if (is_up){
       currentSortOrder = ORDER_ASCENDING;
    }
    else{
       currentSortOrder = ORDER_DESCENDING;
    }
}

function setModelList(list){
    modelList = list
    keyList = [];
    for (var key in modelList){
        keyList.push(key)
    }
}

function sortByIndex(sortIndex, override_order){
    if (override_order !== undefined){
        if (override_order === ORDER_ASCENDING) currentSortOrder = ORDER_ASCENDING
        else if (override_order === ORDER_DESCENDING) currentSortOrder = ORDER_DESCENDING
    }
    currentSortIndex = sortIndex;
    sort();
}

function setupIteration(){
    iterator = 0;
}

function hasNext(){
    return (iterator < keyList.length)
}

function next(){
    if (iterator < keyList.length){
        let ret = modelList[keyList[iterator]];
        iterator++;
        return ret;
    }
    else{
        return modelList[keyList[iterator-1]];
    }
}


function sort(){

    var swap_ocurred = true;
    while (swap_ocurred){
        swap_ocurred = false;
        for (var i = 0; i < keyList.length-1; i++){
            let keyA = keyList[i]
            let keyB = keyList[i+1];
            let compare_result = compareObjects(modelList[keyA],modelList[keyB]);
            if (compare_result === false) return;
            if (compare_result === "swap") {
                swap(i,i+1);
                swap_ocurred = true;
            }
        }
    }

}

function printOutModelListToConsole(){
    for (var i = 0; i < keyList.length; i++){
        console.log(JSON.stringify(modelList[keyList[i]]))
    }

}

function swap(indexA, indexB){
    let A = keyList[indexA]
    keyList[indexA] = keyList[indexB]
    keyList[indexB] = A
}

function compareObjects(A,B){
    if (!(currentSortIndex in A)){
        console.log("ObjectSorter: Object missing sort key '" + currentSortIndex + "': " + JSON.stringify(A))
        return false;
    }
    if (!(currentSortIndex in B)){
        console.log("ObjectSorter: Object missing sort key '" + currentSortIndex + "': " + JSON.stringify(B))
        return false;
    }
    //console.log(currentSortOrder);
    if (currentSortOrder === "descending"){
        if (A[currentSortIndex] < B[currentSortIndex]) return "swap";
    }
    else {
        if (A[currentSortIndex] > B[currentSortIndex]) return "swap";
    }
    return "";

}
