<?php
/**
 * @brief   Class used to encapsulate generating HTML output
 * @date    12.4.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

class HTMLCreator
{
    private $dom;
    private $html;
    private $body;
    private $summaryStyle = 'background-color:lightgray; padding: 5px 10px; text-align: center;';
    private $directoryStyle = 'border-style: dashed;border-color:black; border-radius: 25px; padding-left: 10px; margin-top: 10px;';
    private $singleTestSyle = 'line-height: 0%; margin-left: 3%;';
    private $pageName = 'result.html';

    // Main HTML structure
    function start()
    {
        // <DOCTYPE>
        $imp = new DOMImplementation;
        $dtd = $imp->createDocumentType('html', '', '');
        $this->dom = $imp->createDocument("", "", $dtd);
        $this->dom->encoding = 'UTF-8';

        // <html>
        $this->html = $this->dom->createElement('html');
        $this->dom->appendChild($this->html);

        // <body>
        $this->body = $this->dom->createElement("body");
        $this->html->appendChild($this->body);
    }

    // Summary containing amount of failed, passed and total tests
    function summary($passedCount, $failedCount)
    {
        // summary div
        $div = $this->dom->createElement('div');
        $this->body->appendChild($div);
        $div->setAttribute("style", $this->summaryStyle);

        // summary header
        $summary = $this->dom->createElement('h1');
        $div->appendChild($summary);
        $summary->appendChild($this->dom->createTextNode('Summary'));

        // passed count
        $passed = $this->dom->createElement('p');
        $div->appendChild($passed);
        $passed->appendChild($this->dom->createTextNode('Passed: ' . $passedCount));
        $passed->setAttribute("style", "color:green; font-weight:bold; line-height: 20%");

        // failed count
        $failed = $this->dom->createElement('p');
        $div->appendChild($failed);
        $failed->appendChild($this->dom->createTextNode("Failed: ". $failedCount));
        $failed->setAttribute("style", "color:red; font-weight:bold; line-height: 20%");

        // total count
        $totalCount = $failedCount+$passedCount;
        $total = $this->dom->createElement('p');
        $div->appendChild($total);
        $total->appendChild($this->dom->createTextNode("Total: ". $totalCount));
        $total->setAttribute("style", "font-weight:bold; font-size: large; line-height: 20%");

        // percentage
        $percentage = $this->dom->createElement('h2');
        $div->appendChild($percentage);
        $percentage->appendChild($this->dom->createTextNode(round($passedCount/$totalCount*100, 2) . '%'));
    }

    // Page content containing all directories and tests
    function content($passed, $failed, $all)
    {
        // iterate through all tests and create FAILED and PASSED sections
        foreach ($all as $key => $value) {
            if (!array_key_exists($key, $passed)) {
                $this->single_directory($key, array(), $failed[$key]);
            } else if (!array_key_exists($key, $failed)) {
                $this->single_directory($key, $passed[$key], array());
            } else {
                $this->single_directory($key, $passed[$key], $failed[$key]);
            }
        }
    }

    // Single directory output
    private function single_directory($dirname, $passedTests, $failedTests)
    {
        // directory div
        $div = $this->dom->createElement('div');
        $this->body->appendChild($div);
        $div->setAttribute("style", $this->directoryStyle);

        // directory name header
        $name = $this->dom->createElement('h3');
        $div->appendChild($name);
        $name->appendChild($this->dom->createTextNode($dirname . "\n"));

        // FAILED tests count
        if (count($failedTests)) {
            $boldFailed = $this->dom->createElement('b');
            $div->appendChild($boldFailed);
            $boldFailed->appendChild($this->dom->createTextNode(' FAILED:'));
            $boldFailed->setAttribute("style", "color:red; margin-left: 2%;");
            $div->appendChild($this->dom->createElement('br'));
        }

        // iterate through all failed tests and create separate paragraphs
        foreach ($failedTests as $key => $value) {
            $this->single_test($value, 'red', $div);
        }

        // PASSED tests count
        if (count($passedTests)) {
            $boldPassed = $this->dom->createElement('b');
            $div->appendChild($boldPassed);
            $boldPassed->appendChild($this->dom->createTextNode(' PASSED:'));
            $boldPassed->setAttribute("style", "color:green; margin-left: 2%;");
            $div->appendChild($this->dom->createElement('br'));
        }

        // iterate through all passed tests and create separate paragraphs
        foreach ($passedTests as $key => $value) {
            $this->single_test($value, 'green', $div);
        }
    }

    // Single test paragraph
    private function single_test($testname, $color, &$div)
    {
        $name = $this->dom->createElement('p');
        $name->setAttribute("style", $this->singleTestSyle);
        $div->appendChild($name);
        $name->appendChild($this->dom->createTextNode($testname));
    }

    // Save HTML file
    function end()
    {
        echo $this->dom->saveHTML();
    }

}
?>