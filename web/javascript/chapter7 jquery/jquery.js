$(function(){
    // get html for all ul elements
    var $listHTML = $('ul').html();
    console.log('ul html=' + $listHTML);

    // get text for all ul elements
    var $listText = $('ul').text();
    console.log('ul text=' + $listText);

    // get element by Id
    var $oneEl = $('#one').text();
    console.log('one text=' + $oneEl);

    // update all li elements
    $('li').append('<i> append </i>');

    $listHTML = $('ul').html();
    console.log('ul html=' + $listHTML);

     // update html for all li.hot elements
    $('li.hot').html(function(){
        return '<em>' + $(this).text() + '</em>';
    })

    $listHTML = $('ul').html();
    console.log('ul html=' + $listHTML);

    // update text for all li.hot elements
    $('li.hot').text(function(){
        return $(this).text() + 'updateHot';
    })

    $listHTML = $('ul').html();
    console.log('ul html=' + $listHTML);

    // find li element that contains pine
    $('li:contains("pine")').text('almonds');

    $('li#one').remove();
});