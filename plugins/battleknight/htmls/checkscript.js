


function checkProgress() {
    if(account.isActive()) {

    } else {

    }
}

window.setInterval("checkProgress()",250);

/*
window.addEvent('domready', function() {
    my_notify = new Roar({
        position: 'upperCenter',
        duration: 8000,
        margin: {x: -103, y: 30}
    });


    my_notify.alert('Hello World: ');
});
*/

