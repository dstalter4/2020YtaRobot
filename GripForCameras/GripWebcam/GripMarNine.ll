//
// Inputs
//
Inputs
{
	Mat source0;
}

//
// Variables
//
Outputs
{
	Mat blurOutput;
	Mat rgbThresholdOutput;
	ContoursReport findContoursOutput;
	ContoursReport convexHullsOutput;
	ContoursReport filterContoursOutput;
}

//
// Steps
//

Step Blur0
{
    Mat blurInput = source0;
    BlurType blurType = BOX;
    Double blurRadius = 1.8018018018018014;

    blur(blurInput, blurType, blurRadius, blurOutput);
}

Step RGB_Threshold0
{
    Mat rgbThresholdInput = blurOutput;
    List rgbThresholdRed = [4.850292427924423, 123.42651336243958];
    List rgbThresholdGreen = [158.228417266187, 255.0];
    List rgbThresholdBlue = [87.14028776978414, 211.48464163822527];

    rgbThreshold(rgbThresholdInput, rgbThresholdRed, rgbThresholdGreen, rgbThresholdBlue, rgbThresholdOutput);
}

Step Find_Contours0
{
    Mat findContoursInput = rgbThresholdOutput;
    Boolean findContoursExternalOnly = false;

    findContours(findContoursInput, findContoursExternalOnly, findContoursOutput);
}

Step Convex_Hulls0
{
    ContoursReport convexHullsContours = findContoursOutput;

    convexHulls(convexHullsContours, convexHullsOutput);
}

Step Filter_Contours0
{
    ContoursReport filterContoursContours = convexHullsOutput;
    Double filterContoursMinArea = 400.0;
    Double filterContoursMinPerimeter = 60.0;
    Double filterContoursMinWidth = 25.0;
    Double filterContoursMaxWidth = 1000.0;
    Double filterContoursMinHeight = 15.0;
    Double filterContoursMaxHeight = 1000.0;
    List filterContoursSolidity = [0, 100];
    Double filterContoursMaxVertices = 25.0;
    Double filterContoursMinVertices = 6.0;
    Double filterContoursMinRatio = 1.0;
    Double filterContoursMaxRatio = 15.0;

    filterContours(filterContoursContours, filterContoursMinArea, filterContoursMinPerimeter, filterContoursMinWidth, filterContoursMaxWidth, filterContoursMinHeight, filterContoursMaxHeight, filterContoursSolidity, filterContoursMaxVertices, filterContoursMinVertices, filterContoursMinRatio, filterContoursMaxRatio, filterContoursOutput);
}




