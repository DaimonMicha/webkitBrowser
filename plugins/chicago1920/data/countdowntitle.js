      function countdown(zielzeit,text) {
        var startDatum=new Date(); // Aktuelles Datum
		var startTS = Math.round(startDatum.getTime()/1000);
        var diff= zielzeit - startTS;
        // Countdown berechnen und anzeigen, bis Ziel-Datum erreicht ist
        if(diff > 0)  {

          var tage=0, stunden=0, minuten=0, sekunden=0;

          // Jahre
          tage = Math.floor(diff/(24*60*60));
		  var diffZahl = (tage * 24*60*60);		  
		  stunden = Math.floor((diff-diffZahl)/(60*60));
		  diffZahl += (stunden * 60*60);
		  minuten = Math.floor((diff- diffZahl)/(60));
		  diffZahl += (minuten * 60);
		  sekunden = Math.floor((diff- diffZahl));

          // Anzeige formatieren
          (tage!=1)?tage=tage+" Tage,  ":tage=tage+" Tag,  ";
          (stunden>9)?stunden="["+stunden+":":stunden="[0"+stunden+":";
          (minuten>9)?minuten=minuten+":":minuten="0"+minuten+":";
          (sekunden<10)?sekunden="0"+sekunden+"]":sekunden=sekunden+"]";

          document.title= stunden+minuten+sekunden + " " + text;

          setTimeout("countdown("+zielzeit+",'"+text+"')",200);
        }else{
			document.title= "Chicago1920.com";
		}
      }
	  
	  function countdownRest(restzeit,text) {
		  var startDatum=new Date(); // Aktuelles Datum
		var startTS = Math.round(startDatum.getTime()/1000);
		var endzeit = startTS + restzeit;
        countdown(endzeit,text);
      }
	  
	  

