var kappa= Backbone.Model.extend({	
	
		initialize: function() {
			console.log("kappa active");
			_.bindAll(this, 'newState', 'onNewState');
			this.on("change:seqNum", this.newState);
			this.newStateCallbacks = new Array();
		},
		
		newState: function() {
			/* for compatiblity only */
			var thisDevice = this.get('state');
			thisDevice.assoc.forEach(function(as,index, ar) {
				as.endpoint.forEach(function(ep, index, ar) {
					ep.cluster.forEach(function(cl, index, ar){
						cl.attributeList = cl.attribute;
					});
					ep.clusterList = ep.cluster;	
				});
				as.endpointList = as.endpoint;
			});
			thisDevice.associateList = thisDevice.assoc ;
			/* for compatiblity only */
			
			thisDevice.associates = new Object();
			thisDevice.associateList.forEach(function(as,index, ar){
				thisDevice.associates[as.ieee_id] = as;
				as.endpoints = new Object();
				as.endpointList.forEach(function(ep, index, ar) {
					as.endpoints[ep.id] = ep;
					ep.clusters = new Object();
					ep.clusterList.forEach(function(cl, index, ar){
						ep.clusters[cl.id] = cl;
						cl.attributes = new Object();
						cl.attributeList.forEach(function(at, index, ar){
							cl.attributes[at.id] = at;
						});
					}); 
				});			
			});
			
			this.off("change:seqNum");
			this.set({'thisDevice':thisDevice});
			this.on("change:seqNum", this.newState);
			
			
			this.newStateCallbacks.forEach(
				function(callback, index, ar) {
					callback();
				}
			);			
		},
		
		onNewState: function(callback) {
			this.newStateCallbacks.push(callback);
		}
	});

var Kappa = new kappa();
