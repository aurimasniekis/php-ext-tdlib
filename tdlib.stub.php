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