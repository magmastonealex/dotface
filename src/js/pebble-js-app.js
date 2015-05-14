Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://magmastonealex.github.io/dotface/');
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
    fc=configuration["faceCircle"]
    inv=configuration["inverted"]
	fci=0;
	invi=0;   	
   	if (fc=="T"){
   		fci=1;
   	}
   	if(inv=="T"){
   		invi=1;
   	}
   Pebble.sendAppMessage( { '0': fci, '1': invi },
  function(e) {
    console.log('Successfully delivered message with transactionId='
      + e.data.transactionId);
  },
  function(e) {
    console.log('Unable to deliver message with transactionId='
      + e.data.transactionId
      + ' Error is: ' + e.error.message);
  }
);
  }
);