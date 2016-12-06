

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
    if(currentTime > 0) {
        jQuery("#rivalsProgressRow").show(0);
        jQuery("#rivalsTimer").show(0);
    }
    if(parseInt(jQuery("#rivalsProgress").val()) !== currentTime) {
        jQuery("#rivalsProgress").hide(0);
        jQuery("#rivalsProgress").val(currentTime);
        if(currentTime < allTime) {
            jQuery("#rivalsProgress").show(0);
            jQuery("#rivalsTimer").show(0);
        } else {
            jQuery("#rivalsTimer").hide(0);
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

    var fightsDone = account.opponent("fightsDone") || 0;
    var fightsMax = account.opponent("fightsMax") || 10;
    value = fightsDone + '  - ' + fightsMax;
    if(jQuery("#opponentsProgress").text() !== value) {
        jQuery("#opponentsProgress").text(value);
    }

    var loot = account.opponent("lastLoot") || 0;
    value = ' ' + loot + ' ';
    if(jQuery("#opponentsLoot").text() !== value) {
        jQuery("#opponentsLoot").text(value);
    }

}


function checkGangster() {
    var value;

    value = account.gangster("name");
    if(jQuery("#gangsterName").text() !== value) {
        jQuery("#gangsterName").text(value);
    }

}


function checkTraitor() {
    var value;

    var allTime = 240;
    if(parseInt(jQuery("#traitorsProgress").attr("max")) !== allTime) {
        jQuery("#traitorsProgress").attr("max", allTime);
    }

    var currentTime = parseInt(account.traitor("currentTime")) || 0;
    if(parseInt(jQuery("#traitorsProgress").val()) !== currentTime) {
        jQuery("#traitorsProgress").hide(0);
        jQuery("#traitorsProgress").val(currentTime);
        if(currentTime < allTime) {
            jQuery("#traitorsProgress").show(0);
            jQuery("#traitorsTimer").show(0);
        } else {
            jQuery("#traitorsTimer").hide(0);
        }
    }

    var timeString = account.traitor("timeString");
    if(jQuery("#traitorTimer").text() !== timeString) {
        jQuery("#traitorTimer").text(timeString);
    }

    var fightsMax = parseInt(account.traitor("fightsMax")) || 20;
    var fightsDone = parseInt(account.traitor("fightsDone")) || 0;
    value = fightsDone + ' - ' + fightsMax;
    if(jQuery("#traitorProgress").text() !== value) {
        jQuery("#traitorProgress").text(value);
    }

}



function checkProgress() {
    if(account.isActive()) {
        var path = top.location.pathname;
        if(path.lastIndexOf("battle") < 0) {
            if(account.mustReload()) {
                if(document.title.lastIndexOf("Kampfwartezeit:") < 0) {
                    countdownRest( account.kwz() ,"Kampfwartezeit:");
                    flashOpponent();
                }
            }
        }
    }
    checkGangster();
    checkOpponent();
    checkRival();
    //checkTraitor();
}


function flashOpponent() {
    jQuery("#opponentsTable").addClass("red");
    window.setTimeout(function() {
        jQuery("#opponentsTable").removeClass("red");
        if(++round < 10) {
            window.setTimeout(flashOpponent, 400);
        } else {
            round = 0;
        }
    }, 300);
}

var round = 0;

    jQuery("#rivalsProgressRow").hide(0);
    jQuery("#rivalsTimer").hide(0);
    window.setInterval("checkProgress()",250);

    //var pos = jQuery("table:first").offset();
    //if(pos.left < 152) jQuery("table:first").offset({top:pos.top,left:152});

