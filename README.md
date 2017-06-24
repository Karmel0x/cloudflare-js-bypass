# cloudflare-js-bypass
C++ bypass for Cloudflare's anti-bot page (JavaScript challenge solver). 
Function are using only parser setted unique for this js challenge.
You have to care, cuz if CloudFlare will change their protection, that function will stop working.
working atm (2017-06-06)

## Features
- No dependencies (e.g. no JavaScript engine)
- Easily integrated into your project

## Default Cloudflare message
```
Checking your browser before accessing cloudflare-protected.site.

This process is automatic. Your browser will redirect to your requested content shortly.

Please allow up to 5 seconds…
DDoS protection by Cloudflare
Ray ID: 
```

## Example usage
```
retry:
	std::string r1 = CurlRequest(url);
	if (r1.find("Just a moment") != std::string::npos){// <title>
		std::string JS = solveCFJSChallenge(r1, url);// solver
		Sleep(4000);// mimic waiting process
		CurlRequest(JS, url);// request to CF and save cookies
		goto retry;// retry request without CF challenge
	}
```

## Donations
All donations are unnecessary and not expected, but greatly appreciated.

Bitcoin: 168VLgYZHB8hpSbDjjNt7p5YozSMxGTFcT
