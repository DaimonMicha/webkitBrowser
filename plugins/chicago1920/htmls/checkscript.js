
    if(account.isHeistActive()) {
        jQuery("#heistChecker").prop("checked",true);
    }


    function checkGangster() {
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

    function checkOpponent() {

        if(opponent.id !== parseInt(account.opponent("id"))) {
            opponent.id = parseInt(account.opponent("id"));
        } else {
            return;
        }

        var value;

        value = parseInt(account.opponent("fightsMax")) || 10;
        if(parseInt(jQuery("#opponentsProgress").attr("max")) !== value) {
            jQuery("#opponentsProgress").attr("max", value);
        }

        value = parseInt(account.opponent("fightsDone")) || 0;
        if(parseInt(jQuery("#opponentsProgress").val()) !== value) {
            jQuery("#opponentsProgress").hide(0);
            jQuery("#opponentsProgress").val(value);
            jQuery("#opponentsProgress").show(0);
        }

        value = 'Angriffe: ' + account.opponent("fightsDone") + ' - ' + account.opponent("fightsMax");
        if(jQuery("#opponentsProgress").attr("title") !== value) {
            jQuery("#opponentsProgress").attr("title", value);
        }

        value = account.opponent("name");
        if(jQuery("#opponentsName").text() !== value) {
            jQuery("#opponentsName").text(value);
        }

        value = account.opponent("coded_id");
        jQuery("#opponentsProfile").attr("href", '/characters/profile/' + value)

        value = '(' + account.opponent("level") + ')';
        if($j("#opponentsLevel").text() !== value) {
            $j("#opponentsLevel").text(value);
        }

    }

    function checkProgress() {
        if(account.isActive()) {

            if(account.mustReload()) {

                window.location.reload();
                return;

            }

            checkOpponent();


            //$j("#opponentsProgress").prop("max", parseInt(account.opponent("fightsMax")));
            //$j("#opponentsProgress").prop("value", parseInt(account.opponent("fightsDone")));
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
            // $j("#opponentsName").text(" ");
        }
    }

    window.setInterval("checkProgress()",250);

