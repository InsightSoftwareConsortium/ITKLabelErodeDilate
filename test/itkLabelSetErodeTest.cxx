#include <iomanip>
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkLabelSetErodeImageFilter.h"


template <class MaskPixType, int dim>
int doErode(char *In, char *Out, int radius)
{
  typedef typename itk::Image<MaskPixType, dim> MaskImType;

  // load
  typedef typename itk::ImageFileReader< MaskImType > ReaderType;
  typename ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( In );
  try 
    {
      reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }

  // Label dilation
  typedef typename itk::LabelSetErodeImageFilter<MaskImType, MaskImType> FilterType;
  typename FilterType::Pointer filter = FilterType::New();
  filter->SetInput(reader->GetOutput());
  filter->SetRadius(radius);
  filter->SetUseImageSpacing(true);
  typedef typename itk::ImageFileWriter< MaskImType > WriterType;
  typename WriterType::Pointer writer = WriterType::New();
  writer->SetInput( filter->GetOutput() );
  writer->SetFileName( Out );
  try
    {
      writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
      std::cerr << excp << std::endl;
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;

}

/////////////////////////////////
int readImageInfo(std::string filename, itk::ImageIOBase::IOComponentType *ComponentType, int *dim)
{
  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(filename.c_str(), itk::ImageIOFactory::ReadMode);
  if (imageIO.IsNull())
    return 0;


  imageIO->SetFileName(filename.c_str());
  imageIO->ReadImageInformation();

  *ComponentType = imageIO->GetComponentType();
  *dim = imageIO->GetNumberOfDimensions();
  return(1);
}

int itkLabelSetErodeTest(int argc, char * argv[])
{

  int dim1;
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);
  itk::ImageIOBase::IOComponentType ComponentType;

  if (argc != 4) 
    {
      std::cerr << "Usage: " << argv[0] << "inputimage radius outputimage" << std::endl;
      return(EXIT_FAILURE);
    }

  if (!readImageInfo(argv[1], &ComponentType, &dim1))
    {
    std::cerr << "Failed to open " << argv[1] << std::endl;
    return(EXIT_FAILURE);
    }

  int status = EXIT_FAILURE;
  switch (dim1)
    {
    case 2:
      status=doErode<unsigned char, 2>(argv[1], argv[3], atoi(argv[2]));
      break;
    case 3:
      status=doErode<unsigned char, 3>(argv[1], argv[3], atoi(argv[2]));
      break;
    default:
      std::cerr << "Unsupported dimension" << std::endl;
      return (EXIT_FAILURE);
      break;
    }
  return status;
}