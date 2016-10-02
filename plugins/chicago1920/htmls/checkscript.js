

var opponent = {
    id: 0,
    race: '',
    active: false
}


function checkRival() {

    var allTime = parseInt(account.rival("allTime"));
    if(parseInt(jQuery("#rivalsProgress").attr("max")) !== allTime) {
        jQuery("#rivalsProgress").attr("max", allTime);
    }

    var currentTime = parseInt(account.rival("currentTime")) || 0;
    if(parseInt(jQuery("#rivalsProgress").val()) !== currentTime) {
        jQuery("#rivalsProgress").hide(0);
        jQuery("#rivalsProgress").val(currentTime);
        if(currentTime < allTime) {
            jQuery("#rivalsProgress").show(0);
            jQuery("#rivalsTimer").show(0);
            jQuery("#rivalsCheckerRow").show(0);
        } else {
            jQuery("#rivalsTimer").hide(0);
            jQuery("#rivalsCheckerRow").hide(0);
        }
    }

    var timeString = account.rival("timeString");
    if(jQuery("#rivalTimer").text() !== timeString) {
        jQuery("#rivalTimer").text(timeString);
    }

}


function checkOpponent() {

    if(opponent.id !== parseInt(account.opponent("id"))) {
        opponent.id = parseInt(account.opponent("id"));
    } else {
        //return;
    }

    var value;

    value = account.opponent("name");
    if(jQuery("#opponentsName").text() !== value) {
        jQuery("#opponentsName").text(value);
    }

    value = account.opponent("coded_id");
    jQuery("#opponentsProfile").attr("href", '/characters/profile/' + value)

    value = '(' + account.opponent("level") + ')';
    if(jQuery("#opponentsLevel").text() !== value) {
        jQuery("#opponentsLevel").text(value);
    }

    var fightsMax = account.opponent("fightsMax") || 10;
    var fightsDone = account.opponent("fightsDone") || 0;
    value = fightsDone + ' - ' + fightsMax;
    if(jQuery("#opponentsProgress").text() !== value) {
        jQuery("#opponentsProgress").text(value);
    }

}


function checkGangster() {
}


function checkProgress() {
        if(account.isActive()) {

            if(account.mustReload()) {
                if(document.title.lastIndexOf("Kampfwartezeit:") < 0) {
                    countdownRest( account.kwz() ,"Kampfwartezeit:");
                } else {
                    window.location.reload();
                    return;
                }
            }

            checkGangster();
            checkOpponent();
            checkRival();


        } else {
            // $j("#opponentsName").text(" ");
        }
    }

    window.setInterval("checkProgress()",250);

    //var pos = jQuery("table:first").offset();
    //if(pos.left < 152) jQuery("table:first").offset({top:pos.top,left:152});

