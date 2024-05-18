<?php
/**
 * @brief   class Statistics for bonus extension STATP
 * @date    6.3.2022
 * @author  xzvara01, xzvara01@stud.fit.vutbr.cz
 */

/*
 * class used to store statistics about parsed input file
 * each stats option is stored in its respective counter, which can be
 * modified with various functions
 */
class Statistics
{
    private $_loc          = 0;
    private $_comments     = 0;
    private $_jumps        = 0;
    private $_badjumps     = 0;
    private $_labels_count = 0;
    private $_fw_jumps     = 0;
    private $_back_jumps   = 0;
    private $_labels       = array();   # nested arrays; for each label store another array of positions in code
    private $_jump_targets = array();   # nested arrays; same as labels array

    # loc getter/setter
    public function add_loc() {$this->_loc += 1;}
    public function get_loc() {return $this->_loc;}

    # comments getter/setter
    public function add_comment() {$this->_comments += 1;}
    public function get_comments() {return $this->_comments;}

    # jumps getter
    public function get_jumps() {return $this->_jumps;}

    # forward jumps getter
    public function get_fwjumps(){return $this->_fw_jumps;}

    # back jumps getter
    public function get_backjumps(){return $this->_back_jumps;}

    # label counter getter
    public function get_labels(){return $this->_labels_count;}

    # get badjumps (subtract labels array from jump_targets)
    public function get_badjumps()
    {
        $badjumps = 0;
        foreach (array_diff_key($this->_jump_targets, $this->_labels) as $label) {
            # result is an array of arrays: need to add all badjumps together
            $badjumps += count($label);
        }
        return $badjumps;
    }

    # increment jump counter and add jump target into array
    public function add_jump($target, $position)
    {
        if ($target) { # 'RETURN' instruction does not have a target
            if (array_key_exists($target, $this->_jump_targets)) {
                array_push($this->_jump_targets[$target], $position);
            } else {
                # array has not been created yet
                $this->_jump_targets[$target] = array('0' => $position);
            }
        }
        $this->_jumps += 1;
    }

    # increment label counter and store given label (if it is unique)
    public function add_label($label_name, $position)
    {
        if (array_key_exists($label_name, $this->_labels)) {
            array_push($this->_labels[$label_name], $position);
        } else {
            # array has not been created yet
            $this->_labels[$label_name] = array('0' => $position);
        }
        $this->_labels_count += 1;
    }

    # calculate forward and backward jumps
    public function calculate_fbjumps()
    {
        # iterate through each stored jump, which has positions in code
        # for each position, try to find jump target in labels
        # and for each jump target found decide, whether it is forward or backward jump
        # note that this functions calculates fwjumps and backjumps for labels with same names
        foreach ($this->_jump_targets as $target => $target_positions) {
            foreach ($target_positions as $key1 => $target_location) {
                if (array_key_exists($target, $this->_labels)) {
                    foreach ($this->_labels[$target] as $key2 => $label_location)
                    if ($target_location - $label_location < 0) {
                        $this->_fw_jumps += 1;
                    } else {
                        $this->_back_jumps += 1;
                    }
                }
            }
        }
    }
}

?>