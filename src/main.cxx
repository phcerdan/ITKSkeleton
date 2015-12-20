#include <iostream>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImage.h>

/* #include <itkAnalyzeImageIO.h> */
/* #include <itkJPEGImageIO.h> */
/* #include <itkPNGImageIO.h> */

#include "itkSkeletonizeImageFilter.h"
#include "itkChamferDistanceTransformImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkInvertIntensityImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkConstantPadImageFilter.h"

template<typename ImageType>
bool run(std::string inputFile, std::string outputFile, bool is_white_background = false, bool is_2D = false){
    //READ
    typename itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
    try
    {
        reader->SetFileName(inputFile);
        reader->Update();
        std::clog << "Image read: " << inputFile << std::endl;
    } catch(std::exception & e){
        std::cerr << "Read Failed : " << inputFile << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    typename ImageType::Pointer inputImg = ImageType::New();
    if(is_white_background){
        typedef itk::InvertIntensityImageFilter<ImageType> InverterType;
        typename InverterType::Pointer inverter = InverterType::New();
        inverter->SetInput(reader->GetOutput());

        //MaxCalculator
        typedef itk::MinimumMaximumImageCalculator<ImageType> MinMaxCalcType;
        typename MinMaxCalcType::Pointer maxCalculator = MinMaxCalcType::New();
        maxCalculator->SetImage(reader->GetOutput());
        maxCalculator->Compute();
        typename ImageType::PixelType maximum = maxCalculator->GetMaximum();

        inverter->SetMaximum(maximum);
        inverter->Update();
        inputImg = inverter->GetOutput();
        std::clog << "Image Inverted" << std::endl;

    } else {
        inputImg = reader->GetOutput();

    }

    // PAD
    typedef itk::ConstantPadImageFilter<ImageType, ImageType> PadType;
    typename PadType::Pointer padFilter = PadType::New();
    typename ImageType::SizeType extendRegion;
    for(size_t i = 0 ; i != ImageType::ImageDimension ; ++i ){
        extendRegion[i] = 10;
    }
    typename ImageType::PixelType constPixel = 0;
    padFilter->SetInput(inputImg);
    padFilter->SetConstant(constPixel);
    padFilter->SetPadLowerBound(extendRegion);
    padFilter->SetPadUpperBound(extendRegion);
    padFilter->Update();

    typedef itk::SkeletonizeImageFilter<ImageType, itk::Connectivity<ImageType::ImageDimension, 0> > Skeletonizer;

    /* typedef itk::ConnectedComponentImageFilter<ImageType, ImageType> LabelerType; */
    /* typename LabelerType::Pointer labeler = LabelerType::New(); */
    /* labeler->SetInput(padFilter->GetOutput()); */
    /* labeler->Update(); */
    /* std::clog << "Labeler" << std::endl; */

    /* typedef itk::DanielssonDistanceMapImageFilter<ImageType, ImageType, ImageType> DistanceMapFilterType; */
    /* typename DistanceMapFilterType::Pointer distanceMapFilter = DistanceMapFilterType::New(); */
    /* distanceMapFilter->SetInput(labeler->GetOutput()); */
    /* distanceMapFilter->SetSquaredDistance(true); */
    /* distanceMapFilter->SetInputIsBinary(true); */
    /* distanceMapFilter->Update(); */
    /* std::clog << "Danielsson Distance map generated" << std::endl; */

    typedef itk::ChamferDistanceTransformImageFilter<ImageType,typename  Skeletonizer::OrderingImageType> DistanceMapFilterType;
    typename DistanceMapFilterType::Pointer distanceMapFilter = DistanceMapFilterType::New();
    unsigned int weights[] = { 3, 4, 5 };
    distanceMapFilter->SetDistanceFromObject(false);
    distanceMapFilter->SetWeights(weights, weights+3);
    distanceMapFilter->SetInput(padFilter->GetOutput());
    distanceMapFilter->Update();
    std::clog << "Chamfer Distance map generated" << std::endl;


    typename Skeletonizer::Pointer skeletonizer = Skeletonizer::New();
    skeletonizer->SetInput(padFilter->GetOutput());
    skeletonizer->SetOrderingImage(distanceMapFilter->GetOutput());
    std::clog << "SkeletonPreUpdate" << std::endl;
    skeletonizer->Update();
    typename ImageType::Pointer skeleton = skeletonizer->GetOutput();
    std::clog << "Skeleton generated" << std::endl;

    typedef itk::Image<unsigned short, ImageType::ImageDimension> ShortImageType;
    typedef itk::RescaleIntensityImageFilter<ImageType, ShortImageType > RescalerType;
    typename RescalerType::Pointer rescaler = RescalerType::New();
    rescaler->SetInput(skeleton);
    rescaler->Update();
    std::clog << "Rescaled" << std::endl;

    // WRITE
    typename itk::ImageFileWriter<ShortImageType>::Pointer writer= itk::ImageFileWriter<ShortImageType>::New();
    try{
        writer->SetFileName(outputFile);
        auto o = rescaler->GetOutput();
        o->DisconnectPipeline();
        writer->SetInput(o);
        writer->Update();
    } catch(std::exception & e){
        std::cerr << "WRITE Failed : " << outputFile << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
int main(int argc, char** argv)
{
    if (argc < 3 || argc > 5){
        std::clog << "Usage :: Skeletonize fileToRead fileToWrite [--WB] [--is_2D] " << std::endl;
        std::clog << "Use WB if the input binary image has white background/black foreground" << std::endl;
        return 0;
    }
    std::string fileRead(argv[1]);
    std::string fileOutput(argv[2]);
    bool is_white_background{false};
    if(argc == 4 && std::string(argv[3])=="--WB") is_white_background = true;

    bool is_2D{false};
    if(argc == 5 && std::string(argv[4]) == "--is_2D") is_2D = true;
    if(is_2D){
        unsigned int const Dimension = 2; //is_2D ? 2u : 3u;
        typedef itk::Image<unsigned int, Dimension> ImageType;
        return run<ImageType>(fileRead, fileOutput, is_white_background);
    } else{
        unsigned int const Dimension = 3; //is_2D ? 2u : 3u;
        typedef itk::Image<unsigned int, Dimension> ImageType;
        return run<ImageType>(fileRead, fileOutput, is_white_background);
    }
    return EXIT_SUCCESS;

}
