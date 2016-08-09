//
//  AKAutoWahTests.swift
//  AudioKitTestSuite
//
//  Created by Aurelius Prochazka on 8/9/16.
//  Copyright © 2016 AudioKit. All rights reserved.
//

import XCTest
@testable import AudioKit

class AKAutoWahTests: AKTestCase {

    func testDefault() {
        let input = AKOscillator()
        output = AKAutoWah(input)
        input.start()
        AKTestMD5("")
    }
}
