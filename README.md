# HTTP Client

This is a C++ project for my Communication Protocols course's homework,
where I have designed a HTTP Client library and implemented it for a custom
provided API.

## About

This is a project featuring a HTTP Client library and its usage for a custom
server API. It can perform requests like `Get`, `Post`, `Put` and `Delete`. It
creates a working context called `HttpSession` which handles all HTTP related
fields that may not need to be manually configured by the user every time. This
means headers like cookies and others.

### HttpSession

This creates a context for the user's current HTTP session with a server. The
advantage is that it makes headers like cookies persist and be automatically
populated, also providing an abstraction level for the user.

### HttpConnection

Every request will initialise an object called `HttpConnection` which will
handle the actual data sending. The data should be already formatted in place
and ready to be sent raw. The same applies for receiving. `HttpSession` will
handle traslating the data into a data structure called `HttpResponse`.

### Other custom made libraries

Classes like `Status` or `StatusOr` mostly follow Google's API.
`Reader` provides a nice modular way of reading specific parameters and then
aggregating them in a json format.
`Log` provides logging for the project, including unit tests.