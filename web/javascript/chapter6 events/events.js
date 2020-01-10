//----------------CLICK EVENT -------------------------------------
// if thre is no event object, use old IE event object to get target
function getTarget(e){
    if (!e){
        e = window.event;
    }
    return e.target || e.srcElement;
}

// remove the target li on click
function itemDone(e){
    console.log('itemDone');

    var target, elParent, elGrandparent;
    target = getTarget(e);
    elParent = target.parentNode;
    elGrandparent = target.parentNode.parentNode;

    console.log('target attribute=' + target.getAttribute('href'));
    elGrandparent.removeChild(elParent);

    // prevent default behavior to take user to another page
    // if preventDefault not defined, ues old IE version
    if (e.preventDefault) {
        e.preventDefault();
    }
    else{
        e.returnValue = false;
    }
}

/* set up event listeners to call itemDone on click
   if event listeners not defined, use old IE module
*/
var setupShoppingListClickEvent = (function(){

    var el = document.getElementById("shoppingList");
    if (el.addEventListener){
        el.addEventListener('click', function(e){itemDone(e);}, false);
        console.log('add event listener.');
    }
    else{
        el.attachEvent('onclick', function(e){ itemDone(e);});
        console.log('attach event listener.');
    }
}())

//-----------------LOAD EVENT-------------------------------------

// get username input focus
function setup(){
    console.log('page loaded, setup');

    var textInputEL;
    textInputEL = document.getElementById('password');
    textInputEL.value = '**';
}

/* set up event listeners when page loaded, call setup
*/
var setupLoadFocus = (function(){
    window.addEventListener('load', setup, false);
}())

//-----------------FOCUS/BLUR EVENTS---------------------------------------

// username blur event
function checkUserName(e, minLength){
    var usernameEl = document.getElementById("username");
    var feedbackEl = document.getElementById("user_feedback");
    if (usernameEl.value.length < minLength){
        feedbackEl.className = 'warning';
        feedbackEl.textContent = 'Not long enought, yet...';
    }
    else{
        feedbackEl.textContent = '';
    }
}

// username focus event
function tipUsername(){
    var feedbackEl = document.getElementById("user_feedback");
    feedbackEl.className = '';
    feedbackEl.textContent = 'Username must be at least 5 characters';
}

/* set up event listeners for username input
*/
var addUsernameLicteners = (function(){
    var usernameEl = document.getElementById("username");
    usernameEl.addEventListener('focus', tipUsername, false);
    usernameEl.addEventListener('blur', function(e){ checkUserName(e, 5); }, false); // send event parameters
}())


//----------------MOUSE EVENT----------------------------------------
var sxEl = document.getElementById('screenX');
var syEl = document.getElementById('screenY');
var pxEl = document.getElementById('pageX');
var pyEl = document.getElementById('pageY');
var cxEl = document.getElementById('clientX');
var cyEl = document.getElementById('clientY');

function showPosition(event){
    sxEl.textContent = event.screenX;
    syEl.textContent = event.screenY;
    pxEl.textContent = event.pageX;
    pyEl.textContent = event.pageY;
    cxEl.textContent = event.clientX;
    cyEl.textContent = event.clientY;
}

var addMousemoveEvent = (function(){
    var el = document.getElementById('body');
    el.addEventListener('mousemove', showPosition, false);
}())


//----------------KEYBOARD EVENT----------------------------------------

function charCount(e){
    var textEntered, charDispaly, counter, lastKey;
    textEntered = document.getElementById('message').value;
    charDispaly = document.getElementById('charactersLeft');
    counter = (50 - textEntered.length);
    charDispaly.textContent = counter;

    if (counter <= 1 ){
        document.getElementById('message').value = textEntered.substring(0, 49);
    }

    
    lastKey = document.getElementById('lastKey');
    lastKey.textContent = 'Last key in ASCII code: ' + String.fromCharCode(e.keyCode);
}

var addKeybouardEvent = (function(){

    console.log('addKeybouardEvent');

    var el = document.getElementById('message');
    el.addEventListener('keypress', charCount, false);
    // it can use input event, but it wont detect keyCode. Input can detect copy-paste
    //el.addEventListener('input', charCount, false);
}())

//----------------BUTTON EVENT, MUTATION EVENT----------------------------------------

function addItem(e){
    e.preventDefault();
    var newEl, newText, listEl;
    newEl = document.createElement('li');
    newText = document.createTextNode('New list item');
    newEl.appendChild(newText);

    listEl = document.getElementById('shoppingList');
    listEl.appendChild(newEl);
}

function updateCount(){
    var listItems = document.getElementsByTagName('li').length;
    var listCountEl = document.getElementById('listCount');
    listCountEl.textContent = 'Item count=' + listItems;
}

var addButtonEvent = (function(){
    var buttonEl, listEl;
    buttonEl = document.getElementById('addItemBtn');
    buttonEl.addEventListener('click', addItem, false);

    listEl = document.getElementById('shoppingList');
    listEl.addEventListener('DOMNodeInserted', updateCount, false);

    updateCount();
}())



//----------------HTML5 EVENTS----------------------------------------

function domSetup(){
    console.log('DOMContentLoaded');
    var textInput = document.getElementById('message');
    textInput.focus();
}

var html5Events = (function(){

    window.addEventListener('DOMContentLoaded', domSetup, false);
    window.addEventListener('beforeunload', function(event){ return 'You have changes that not saved..';}, false);
}())