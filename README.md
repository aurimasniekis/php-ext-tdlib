# TdLib PHP Extension

A PHP extension to integrate [TdLib][tdlib-url] via JSON interface


## Requirements

* PHP 7.0+

## Installation

    phpize
    ./configure
    make
    make install

Add the following line to your `php.ini`

    extension=tdlib.so

To try out the extension, you can run the following command

    php -a -d extension=modules/tdlib.so
    

## Reference

```php
<?php

class TdLib
{
    /**
     * Synchronously executes TDLib request.
     * Only a few requests can be executed synchronously.
     *
     * @param array|\JsonSerializable $request
     *
     * @return array
     */
    public static function execute($request): array
    {
    }

    /**
     * Receives incoming updates and request responses from the TDLib client.
     *
     * @param float $timeout The maximum number of seconds allowed for this function to wait for new data.
     *
     * @return array
     */
    public function receive(float $timeout): array
    {
    }

    /**
     * Sends request to the TDLib client.
     *
     * @param array|\JsonSerializable $request
     */
    public function send($request): void
    {
    }
}
```

## Contributing

Please see [CONTRIBUTING](CONTRIBUTING.md) and [CONDUCT](CONDUCT.md) for details.


## License

Please see [License File](LICENSE) for more information.

[tdlib-url]: https://github.com/tdlib/td