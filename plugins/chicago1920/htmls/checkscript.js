
    if(account.isHeistActive()) {
        jQuery("#heistChecker").prop("checked",true);
    }

    var opponent = {
        id: 0,
        name: '',
        race: '',
        level: 0,
        fightsDone: 0,
        fightsMax: 10,
        active: false,
        coolDown: ''
    }

    function checkProgress() {
        if(account.isActive()) {
            var value;

            if(account.mustReload()) window.location.reload();
            if(opponent.id !== parseInt(account.opponent("id"))) {
                opponent.id = parseInt(account.opponent("id"));
            }

            var text = '0 - 10';
            value = parseInt(account.opponent("fightsMax")) || 10;
            //if(value === 0) value = 10;
            if(parseInt(jQuery("#opponentsFights").attr("max")) !== value) {
                jQuery("#opponentsFights").attr("max", value);
            }

            value = parseInt(account.opponent("fightsDone")) || 0;
            if(jQuery("#opponentsFights").val() !== value) {
                jQuery("#opponentsFights").val(value);
                text = value + ' - ';
            }
            jQuery("#opponentsRace").text(text);

            value = '(' + account.opponent("coded_id") + ') ' + account.opponent("name");
            if(jQuery("#opponentsName").text() !== value) {
                jQuery("#opponentsName").text(value);
            }
            value = account.opponent("level");
            if($j("#opponentsLevel").text() !== value) {
                $j("#opponentsLevel").text(value);
            }

            //$j("#opponentsFights").prop("max", parseInt(account.opponent("fightsMax")));
            //$j("#opponentsFights").prop("value", parseInt(account.opponent("fightsDone")));
            //$j("#opponentsName").text(account.opponent("name"));
            //$j("#opponentsLevel").text(account.opponent("level"));

            //$j("#opponentsName").text("aktiv");
            var pageTitle = account.workingTitle();
            // 'http://www.chicago1920.com/npc'
            if(pageTitle !== document.title) {
                // bitte nur einmal:
                //if(document.title === 'Chicago1920.com') window.location.reload();
                //document.title = pageTitle;
            }
        } else {
            $j("#opponentsName").text(" ");
        }
    }

    window.setInterval("checkProgress()",250);

