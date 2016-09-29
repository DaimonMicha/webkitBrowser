


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
            //return;
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
        if(jQuery("#opponentsLevel").text() !== value) {
            jQuery("#opponentsLevel").text(value);
        }

    }

    function checkRival() {
        var value;

        value = parseInt(account.rival("allTime"));
        if(parseInt(jQuery("#rivalsProgress").attr("max")) !== value) {
            jQuery("#rivalsProgress").attr("max", value);
        }

        value = parseInt(account.rival("currentTime")) || 0;
        if(parseInt(jQuery("#rivalsProgress").val()) !== value) {
            jQuery("#rivalsProgress").hide(0);
            jQuery("#rivalsProgress").val(value);
            jQuery("#rivalsProgress").show(0);
        }

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

            checkOpponent();
            checkRival();


        } else {
            // $j("#opponentsName").text(" ");
        }
    }

    window.setInterval("checkProgress()",250);

