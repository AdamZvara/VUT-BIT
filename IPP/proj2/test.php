<?php
/**
* @brief   Main testing script
* @date    12.4.2022
* @author  xzvara01, xzvara01@stud.fit.vutbr.cz
*/

include "context.php";
include "testmodules.php";
include "htmlcreator.php";

// Class to search for testing files based on given directory and recursive option
class DirectorySearch
{
    private $_iterator;

    // Create recursive iterator to traverse thourgh directories
    function __construct($directory, $recursive)
    {
        $dictIterator = new RecursiveDirectoryIterator($directory);
        $this->_iterator = new RecursiveIteratorIterator($dictIterator);

        // recursive option not set - search only through given directory
        if ($recursive == null)
            $this->_iterator->setMaxDepth(0);
    }

    // Get next file in directory (subdirectories) that has '.src' extension
    function get_file()
    {
        if ($this->_iterator->valid() == true) {
            $file = $this->_iterator->current();
            $this->_iterator->next();
            if ($file->getExtension() == 'src') {
                $pathname = $file->getPathname();
                if (!$file->isDir())
                    return $pathname;
                else
                    return $this->get_file();
            }
            return $this->get_file();
        } else {
            return false;
        }
    }
}

// Find (or create) files used for testing (in, rc, out)
function check_files($source)
{
    $basePathName = substr($source, 0, -4);

    # rc file check
    $pathname = $basePathName . '.rc';
    if (!file_exists($pathname))
        file_put_contents($pathname, 0);

    # in file check
    $pathname = $basePathName . '.in';
    if (!file_exists($pathname))
        file_put_contents($pathname, '');

    # out file check
    $pathname = $basePathName . '.out';
    if (!file_exists($pathname))
        file_put_contents($pathname, '');
}

// Add key value to array
function add_to_array($fileName, $directoryName, &$array)
{
    if (array_key_exists($directoryName, $array)) {
        array_push($array[$directoryName], $fileName);
    } else {
        $array[$directoryName] = array($fileName);
    }
}

$context   = new Context($argc);
$dirsearch = new DirectorySearch($context->directory, $context->recursive);

Tester::set_options($context->parse_script, $context->int_script, $context->jexampath, $context->clean);
$parseTester     = new ParseTester();
$interpretTester = new InterpretTester();
$multiTester     = new MultiTester();

// 2D arrays
$testsPassed = array();
$testsFailed = array();
// 1D array
$allTests = array();

// Actual testing
while ($file = $dirsearch->get_file()) {
    check_files($file);

    $separatorPos = strrpos($file, '/') + 1;
    $directoryName = substr($file, 0, $separatorPos);
    $fileName = substr($file, $separatorPos);

    if ($context->parse_only) {
        $retval = $parseTester->test($file);
    } else if ($context->int_only) {
        $retval = $interpretTester->test($file);
    } else {
        $retval = $multiTester->test($file);
    }

    if ($retval) {
        add_to_array($fileName, $directoryName, $testsPassed);
    } else {
        add_to_array($fileName, $directoryName, $testsFailed);
    }

    if (!array_key_exists($directoryName, $allTests)){
        $allTests[$directoryName] = false;
    }
}

// count elements recursively and subtract the first layer (keys only)
$passedCount = count($testsPassed, COUNT_RECURSIVE) - count($testsPassed);
$failedCount = count($testsFailed, COUNT_RECURSIVE) - count($testsFailed);

// sort directories by name
ksort($allTests);

// HTML generation
$html = new HTMLCreator();
$html->start();
$html->summary($passedCount, $failedCount);
$html->content($testsPassed, $testsFailed, $allTests);
$html->end();
?>
