var msg_welcome = "select product";
var msg_success = "vending product";
var msg_error = "invalid code";
var msg_instock = "Press # to vend";
var msg_outofstock = "out of stock";
var msg_thanks = "Thank you";
var id;

$(document).ready(function($){
    window.scrollTo(0, 1);
	clearDisplay();    
    setDisplay(msg_welcome);
    
var displaypos = $('#display').offset().top;
	

$('.cancel').click(function() {
	var displaytext = $('#display').html();
	
	if(!isDisplayNumeric(displaytext))
		clearDisplay();
	else{
		var newdisplaytext = displaytext.substring(0, displaytext.length - 1);
		setDisplay(newdisplaytext);
	}

});

$('.submit').click(function() {
	var displaytext = $('#display').html();
	
	if (displaytext == msg_instock) {
		$.getJSON('vend', {id: id}, function(data) {
			if ('err' in data) {
				setDisplay(data['err']);
			}
			
			else if ('res' in data) {
				setDisplay(msg_success);
				setTimeout(function(){setDisplay(msg_thanks)},3000);	
			}
		});	
	}
		
	else if (displaytext == msg_outofstock) {
		setDisplay(msg_outofstock);
	}
		
	else {
		id = parseInt(displaytext);

		$.getJSON('stock', {id: id}, function(data) {
			if ('err' in data) {
				setDisplay(data['err']);
			}
			
			else if ('res' in data) {
				switch (data['res']) {
					case 0: setDisplay(msg_outofstock); break;
					default: 
						var msg = data['res'] + ' left: ' + msg_instock;
						setDisplay(msg);
				}	
			}
		});			
	}
	
	// if(displaytext == "0001")
		// setDisplay(msg_instock);
		
	// else if(displaytext == "0002")
		// setDisplay(msg_outofstock);
		
	// else 
		
	// $('html, body').animate({scrollTop:displaypos}, 'slow');
	
});
    
$('.key').click(function() {

       var key = $(this).attr("value");
       var displaytext = $('#display').html();
       
       

       
       if(!isDisplayNumeric(displaytext))
       		clearDisplay();
       		
       	
       	var length = displayLength();  

       		
       
      if(length < 4 | length == 0)
       		appendDisplay(key);
       
       
       
       
        });


});

window.addEventListener('load', function() {
    new FastClick(document.body);
}, false);

/* Functions */

function setDisplay(string){
	$('#display').html(string);
}

function appendDisplay(string){
	$('#display').append(string);
}

function clearDisplay(){
	$('#display').html("");
}

function isDisplayNumeric(input){

var b = ((input - 0) == input && input.length <= 4);
	return b ;
}

function displayLength(){
	return $('#display').html().length;
}