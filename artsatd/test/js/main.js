// =============================================================================
//
// Copyright (c) 2009-2013 Christopher Baker <http://christopherbaker.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// =============================================================================

var JSONRPCClient;
var g_session = '';

function setSession(session) {
    g_session = session;
    if (session == '') {
        session = '****************';
    }
    $('#session').text(session);
}

function getSession() {
    return g_session;
}

function initializeButtons() {
    $('#rpcEcho').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('system.rpcEcho',
            {
                'session': getSession(),
                'param': $('#rpcEcho-param').val()
            },
            function(result) {
                setSession(result['session']);
                $('#rpcEcho-param').val(JSON.stringify(result));
            },
            function(error) {
                $('#rpcEcho-param').val(JSON.stringify(error));
            });
    });

    $('#getVersion').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getVersion',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getVersion-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getVersion-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSession').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSession',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSession-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSession-result').val(JSON.stringify(error));
            });
    });
    
    $('#getManualRotator').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getManualRotator',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getManualRotator-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getManualRotator-result').val(JSON.stringify(error));
            });
    });
    
    $('#getManualTransceiver').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getManualTransceiver',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getManualTransceiver-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getManualTransceiver-result').val(JSON.stringify(error));
            });
    });
    
    $('#getManualTNC').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getManualTNC',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getManualTNC-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getManualTNC-result').val(JSON.stringify(error));
            });
    });
    
    $('#getNORAD').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getNORAD',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getNORAD-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getNORAD-result').val(JSON.stringify(error));
            });
    });
    
    $('#getMode').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getMode',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getMode-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getMode-result').val(JSON.stringify(error));
            });
    });
    
    $('#getTime').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getTime',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getTime-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getTime-result').val(JSON.stringify(error));
            });
    });
    
    $('#getObserverCallsign').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getObserverCallsign',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getObserverCallsign-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getObserverCallsign-result').val(JSON.stringify(error));
            });
    });
    
    $('#getObserverPosition').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getObserverPosition',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getObserverPosition-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getObserverPosition-result').val(JSON.stringify(error));
            });
    });
    
    $('#getObserverDirection').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getObserverDirection',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getObserverDirection-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getObserverDirection-result').val(JSON.stringify(error));
            });
    });
    
    $('#getObserverFrequency').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getObserverFrequency',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getObserverFrequency-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getObserverFrequency-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftPosition').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftPosition',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftPosition-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftPosition-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftDirection').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftDirection',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftDirection-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftDirection-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftDistance').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftDistance',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftDistance-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftDistance-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftSpeed').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftSpeed',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftSpeed-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftSpeed-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftFrequency').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftFrequency',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftFrequency-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftFrequency-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftDopplerShift').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftDopplerShift',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftDopplerShift-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftDopplerShift-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftAOSLOS').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftAOSLOS',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftAOSLOS-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftAOSLOS-result').val(JSON.stringify(error));
            });
    });
    
    $('#getSpacecraftMEL').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getSpacecraftMEL',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getSpacecraftMEL-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getSpacecraftMEL-result').val(JSON.stringify(error));
            });
    });
    
    $('#getRotatorStart').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getRotatorStart',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getRotatorStart-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getRotatorStart-result').val(JSON.stringify(error));
            });
    });
    
    $('#getError').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.getError',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#getError-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#getError-result').val(JSON.stringify(error));
            });
    });
    
    $('#isValidRotator').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.isValidRotator',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#isValidRotator-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#isValidRotator-result').val(JSON.stringify(error));
            });
    });
    
    $('#isValidTransceiver').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.isValidTransceiver',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#isValidTransceiver-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#isValidTransceiver-result').val(JSON.stringify(error));
            });
    });
    
    $('#isValidTNC').on('click', function() {
        var $this = $(this);
        JSONRPCClient.call('observer.isValidTNC',
            {
                'session': getSession()
            },
            function(result) {
                setSession(result['session']);
                $('#isValidTNC-result').val(JSON.stringify(result));
            },
            function(error) {
                $('#isValidTNC-result').val(JSON.stringify(error));
            });
    });
}

$(document).ready(function() {
    JSONRPCClient = new $.JsonRpcClient({
        'ajaxUrl': 'http://localhost:16782/rpc.json'
    });
    initializeButtons();
});
