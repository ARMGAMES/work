
var func1 = (function(){
    console.log("js start here.");

    var getEl1 = document.getElementById("three");
    if (getEl1){
        console.log("found three.");
        console.log("three attribute=" + getEl1.getAttribute('class'));
    }
    else{
        console.log("no three.");
    }
    
    var getEl2 = document.getElementsByTagName("li");
    if (getEl2){
        console.log("found li " + getEl2.length);
        console.log("first child textContent=" + getEl2[0].textContent);
        console.log("first child innerText=" + getEl2[0].innerText);
        console.log("first child innerHTML=" + getEl2[0].innerHTML);
    }

    var getEl3 = document.querySelectorAll("li.hot");
    if (getEl3){
        console.log("found li.hot " + getEl3.length);
    }

    getEl2[1].textContent = "ipad";
    console.log("old 2 text= " + getEl3[1].textContent);

    document.getElementsByTagName("li")[1].className = "cool";

    var newEl = document.createElement("li");
    newEl.className = "hot";
    var newText = document.createTextNode("quinoa");
    newEl.appendChild(newText);

    var position = document.getElementsByTagName("ul")[0];
    position.appendChild(newEl);

    var removeEl = document.getElementById("three");
    var containerEl = removeEl.parentNode;
    containerEl.removeChild(removeEl);

    document.getElementById("one").removeAttribute("class");
    console.log("ONE ATTRIBUE=" + document.getElementById("one").getAttribute("class"));

    document.getElementById("one").setAttribute("class", "complete");
}())