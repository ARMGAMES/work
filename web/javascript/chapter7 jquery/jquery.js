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

    //$('li#one').remove();

    // update attributes
    $('li#one').removeClass();
    $('li#one').addClass('hot');

    $('li#one').attr('id', 'five');
    $('li#five').attr('id', 'one');
    //$('li#five').removeAttr('id');

    // get/set css rules
    var coler = $('li#one').css('color');
    console.log('id one color=' + coler);

    $('li').css('padding-left', '+=20');
    $('li').css({
        'background-color': '#d3d3d3',
        'font-family' : 'Courier'
    });

    $listHTML = $('ul').html();
    console.log('ul html=' + $listHTML);

    // .each() loop thru all elements
    $('li').each(function(){
        var ids = this.id;
        $(this).append('<em class="complete">' + ' ' + ids + '</em>');
    });

    // events
    var $listItems = $('li');
    $listItems.on('mouseover', function(e){
        var ids = this.id;
        $listItems.children('span').remove();
        $(this).append(' <span class="mouseon">' + ids + ' ' + e.type + '</span>');
        //console.log("current li=" + $(this).html());
    });

    $listItems.on('mouseout', function(){
        $listItems.children('span').remove();
    });

   /* $('ul').on('click', ':not(#four)', {status: 'important'}, function(e){
        var listItem = 'item: ' + e.target.textContent + '<br />';
        var itemStatus = 'Status: ' + e.data.status + '<br />';
        var eventType = 'Event: ' + e.type;
        $('#notes').html(listItem + itemStatus + eventType);
    });
    */

    $('li').hide().each(function(index){
        $(this).delay(700 * index).fadeIn(700);
    });

    // click to remove with animation
    $('li').on('click', function(){
        $(this).animate(
            {   opacity: 0.0,
                paddingLeft: '+=80'
            },
            500, function(){
                $(this).remove();
        });
    });
});