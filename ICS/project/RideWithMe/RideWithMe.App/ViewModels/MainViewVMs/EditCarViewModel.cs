using System;
using System.Data;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels.MainViewVMs;

public class EditCarViewModel: ViewModelBase, IEditCarViewModel
{
    private readonly IMediator _mediator;

    public EditCarViewModel(
        ICarListViewModel carListViewModel,
        IMediator mediator)
    {
        CarListViewModel = carListViewModel;
        _mediator = mediator;

        EditCarCommand = new RelayCommand<CarListModel?>(EditCar);
    }

    public ICarListViewModel CarListViewModel { get; set; }
    public ICommand EditCarCommand { get; }

    private void EditCar(CarListModel? selectedCar)
    {
        if (selectedCar != null)
        {
            _mediator.Send(new OpenCarDetailMessage<CarWrapper> {Id = selectedCar.Id} );
        }
    }
}