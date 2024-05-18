using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;
using Microsoft.Toolkit.Mvvm.Input;
using RideWithMe.App.Extensions;
using RideWithMe.App.Messages;
using RideWithMe.App.Messages.ViewMessages;
using RideWithMe.App.Services;
using RideWithMe.App.ViewModels.Interfaces;
using RideWithMe.App.Wrappers;
using RideWithMe.BL.Facades;
using RideWithMe.BL.Models;

namespace RideWithMe.App.ViewModels.MainViewVMs
{
    public class CarListViewModel : ViewModelBase, ICarListViewModel
    {
        private readonly CarFacade _carFacade;
        private readonly ILoggedInUser _loggedInUser;
        private readonly IMediator _mediator;
        public CarListViewModel(
            CarFacade carFacade,
            ILoggedInUser loggedInUser,
            IMediator mediator)
        {
            _carFacade = carFacade;
            _loggedInUser = loggedInUser;
            _mediator = mediator;


            _mediator.Register<UpdateMessage<CarWrapper>>(LoadOwnerCars);
        }

        private CarListModel? _selectedModel;
        public CarListModel? SelectedModel
        {
            get => _selectedModel;
            set
            {
                _selectedModel = value;
                OnSelectionChanged(value);
            }
        }
        private void OnSelectionChanged(CarListModel? model)
        {
            _mediator.Send<SelectedMessage<CarWrapper>>(new() { Id = model?.Id});
        }

        public ObservableCollection<CarListModel> Cars { get; } = new();

        public int CarCount { get; set; }

        private async void LoadOwnerCars(UpdateMessage<CarWrapper> _)
        {
            Guid logged = _loggedInUser.GetLoggedUserGuid();
            Cars.Clear();
            var cars = await _carFacade.GetAllOwnerAsync(logged);
            Cars.AddRange(cars);
            CarCount = Cars.Count;
        }
    }
}
