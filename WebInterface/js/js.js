var msg_welcome = "select products";
var msg_success = "vending product";
var msg_error = "invalid code";
var msg_instock = "press # to vend";
var msg_outofstock = "out of stock";
var msg_outofstock = "out of credit";
var msg_thanks = "thank you";
var credit_code = 99; //number to increase your credit
var id;

$(document).ready(function($){
    window.scrollTo(0, 1);
	clearDisplay();    
    setDisplay(msg_welcome);
    
var displaypos = $('#displaymsg').offset().top;
	

/* [*] key */
$('.cancel').click(function() {
	var displaytext = $('#displaymsg').html();
	
	//clear any messages
	if(!isDisplayNumeric(displaytext))
		clearDisplay();
		
	//remove last digit	
	else{
		var newdisplaytext = displaytext.substring(0, displaytext.length - 1);
		setDisplay(newdisplaytext);
	}

});

/* [#] key */
$('.submit').click(function() {
	var displaytext = $('#displaymsg').html();
	
	//if the user wants to increase their credit
	if(displaytext == credit_code){	
		$.getJSON('addcredit', {id: id}, function(data) {
			if ('res' in data) {
				setDisplay(data['res']);	
			}
		});
	}
	

	
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
		
	else if (displaytext == msg_outofstock);
	
	//request stock
	else {
		id = parseInt(displaytext);

		$.getJSON('stock', {id: id}, function(data) {
			if ('err' in data) {
				setDisplay(data['err']);
			}
			
			else if ('res' in data) {
				switch (data['res']) {
					case -1: setDisplay(msg_outofcredit); break;
					case 0: setDisplay(msg_outofstock); break;
					default: 
						var msg = data['res'] + ' left: ' + msg_instock;
						setDisplay(msg);
				}	
			}
		});			
	}
	
	
});
    
$('.key').click(function() {

       var key = $(this).attr("value");
       var displaytext = $('#displaymsg').html();
       
       

       
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
	$('#displaymsg').html(string);
}

function appendDisplay(string){
	$('#displaymsg').append(string);
}

function clearDisplay(){
	$('#displaymsg').html("");
}

function isDisplayNumeric(input){

var b = ((input - 0) == input && input.length <= 4);
	return b ;
}

function displayLength(){
	return $('#displaymsg').html().length;
}
